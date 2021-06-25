<?php
/*& (ampersand) becomes '&amp;'
" (double quote) becomes '&quot;'
' (singe quote) becomes '&#039;'
< (less than) becomes '&lt;'
> (greater than) becomes '&gt;'*/

/*$foreignKeyConstrains = $this->fetchResult(
    "SELECT"
    . " TABLE_NAME,"
    . " COLUMN_NAME,"
    . " REFERENCED_TABLE_NAME,"
    . " REFERENCED_COLUMN_NAME"
    . " FROM information_schema.key_column_usage"
    . " WHERE referenced_table_name IS NOT NULL"
    . " AND TABLE_SCHEMA = '" . $this->escapeString($database) . "'"
    . " AND TABLE_NAME IN (" . $tablesListForQuery . ")"
    . " AND REFERENCED_TABLE_NAME IN (" . $tablesListForQuery . ");",*/

class db2php
{
    private $db_tables = [];
    private $db_link = null;
    private $database = null;

    function __construct($host, $username, $password, $database, $port=3306)
    {
        $this->database = $database;
        $this->db_link = new Database($host, $username, $password, $database, $port);
        if (!($this->db_link = new Database($host, $username, $password, $database, $port)))
        {
            echo "ERROR sql db2php db_link \r\n";
            return;
        }
        /*get all tables*/
        $tables = $this->db_link->query("select table_name from information_schema.tables WHERE table_schema = ?", $this->database);
        if($tables === false)
        {
            echo "ERROR sql in db2php \r\n";
        }
        $db_tables_info = [];
        for ($i=0; $i<count($tables); $i++)
        {
            $table_name = $tables[$i]["TABLE_NAME"];
            $db_tables_info[$table_name] = [];
        }
        for ($i=0; $i<count($tables); $i++)
        {
            $table_name = $tables[$i]["TABLE_NAME"];
            $columns = $this->db_link->query("SELECT COLUMN_NAME FROM information_schema.columns WHERE table_name = ?", $table_name);
            for ($j=0; $j<count($columns); $j++)
            {
                array_push($db_tables_info[$table_name], $columns[$j]["COLUMN_NAME"]);
            }
        }
        $this->db_tables = $db_tables_info;
    }

    function generate()
    {
        $tables = array_keys($this->db_tables);
        $code = "";
        $code .= $this->generate_namespace($this->database);
        $code .= $this->generate_query_class();
        $code .= $this->generate_database_class($this->database, $tables);
        $code .= $this->generate_table_calass();
        foreach ($tables as $table)
        {
            $code .= $this->generate_table_row_calass($table, $this->db_tables[$table]);
        }
        return $code;
    }
/************************************************row_class*************************************************************/
    function generate_table_row_calass($table, array $columns)
    {
        $code = "";
        $code .= $this->generate_table_row_init($table);
        $code .= $this->generate_column_row_getall($table);
        $code .= $this->generate_column_row_exist($table);
        $code .= $this->generate_column_row_delete($table);
        for ($i=0; $i<count($columns); $i++)
        {
            $code .= $this->generate_column_row_setter($table, $columns[$i]);
            $code .= $this->generate_column_row_getter($table, $columns[$i]);
        }
        $code .= "}";
        return $code;
    }

    function generate_table_row_init($table)
    {
        $code = "
class {$table}_row
{
    private \$ID = null;
    private \$db_link = null;
    function __construct(\$ID, \$db_link)
    {
        \$this->ID = \$ID;
        \$this->db_link = \$db_link;
    }\n";
        return $code;
    }

    function generate_column_row_setter($table, $column)
    {
        $code = "
    function set_".$column."(\$value)
    {
        \$status = \$this->db_link->query('UPDATE ".$table." SET ".$column." = ? WHERE ID = ?', \$value, \$this->ID);
        if(\$status === false)
        {
            echo 'ERROR sql in set_".$column." \\r\\n';
        }";
        if ($column == "ID")
        {
            $code.="
        else
        {
            \$this->ID = \$value;
        }";
        }
        $code.="
        return \$status;
    }\n";
        return $code;
    }

    function generate_column_row_getter($table, $column)
    {
        $code = "
    function get_".$column."()
    {
        \$status = \$this->db_link->query('SELECT ".$column." FROM ".$table." WHERE ID = ?', \$this->ID);
        if(\$status === false)
        {
            echo 'ERROR sql in get_".$column." \\r\\n';
        }
        return \$status;
    }\n";
        return $code;
    }

    function generate_column_row_getall($table)
    {
        $code = "
    function getall() : array
    {
        \$status = \$this->db_link->query('SELECT * FROM ".$table." WHERE ID = ?', \$this->ID);
        if(\$status === false)
        {
            echo 'ERROR sql in getall \\r\\n';
        }
        return \$status;
    }\n";
        return $code;
    }


    function generate_column_row_delete($table)
    {
        $code = "
    function delete()
    {
        \$status = \$this->db_link->query('DELETE FROM ".$table." WHERE ID = ?', \$this->ID);
        if(\$status === false)
        {
            echo 'ERROR sql in delete_".$table." \\r\\n';
        }
        return \$status;
    }\n";
        return $code;
    }

    function generate_column_row_exist($table)
    {
        $code = "
    function exist()
    {
        \$status = \$this->db_link->query('SELECT ID FROM ".$table." WHERE ID = ?', \$this->ID);
        if(\$status === false)
        {
            echo 'ERROR sql in exist \\r\\n';
            return null;
        }
        elseif (\$status === null)
        {
            return false;
        }
        else
        {
            return true;
        }
    }\n";
        return $code;
    }
/************************************************table_class***********************************************************/
    function generate_table_calass()
    {
        $code = "";
        $code .= $this->generate_table_init();
        $code .= $this->generate_row_get_by_id();
        $code .= $this->generate_row_get_by();
        $code .= $this->generate_row_get_all();
        $code .= $this->generate_row_inserter();
        $code .= "}";
        return $code;
    }

    function generate_table_init()
    {
        $code = "
class table_manager
{
    private \$table_name = null;
    private \$row_class = null;
    private \$db_link = null;
    
    function __construct(\$db_link, \$table_name, \$row_class)
    {
        \$this->table_name = \$table_name;
        \$this->row_class = \$row_class;
        \$this->db_link = \$db_link;
    }\n";
        return $code;
    }

    function generate_row_get_by_id()
    {
        $code = "
    function get_by_id(\$id)
    {
        \$row = new \$this->row_class(\$id, \$this->db_link);
        \$status = \$row->exist();
        if(\$status === true)
        {
            return \$row;
        }
        else
        {
            return \$status;
        }
    }\n";
        return $code;
    }

    function generate_row_get_by()
    {
        $code = "
    function get_by(\$sql, ...\$args)
    {
        \$sql = \"SELECT ID FROM {\$this->table_name} WHERE \".\$sql;
        array_unshift(\$args, \$sql);
        \$ID_array = call_user_func_array(array(\$this, 'db_link', 'query'), \$args);
        if (\$ID_array === false)
        {
            return false;
        }
        \$rows = [];
        foreach(\$ID_array as \$ID)
        {
            \$row = new \$this->row_class(\$ID['ID'], \$this->db_link);
            array_push(\$rows, \$row);
        }
        return \$rows;
    }\n";
        return $code;
    }

    function generate_row_get_all()
    {
        $code = "
    function get_all()
    {
        \$sql = \"SELECT * FROM {\$this->table_name}\";
        \$ID_array = \$this->db_link->query(\$sql);
        if (\$ID_array === false)
        {
            return false;
        }
        \$rows = [];
        foreach(\$ID_array as \$ID)
        {
            \$row = new \$this->row_class(\$ID['ID'], \$this->db_link);
            array_push(\$rows, \$row);
        }
        return \$rows;
    }\n";
        return $code;
    }

    function generate_row_inserter()
    {
        $code = "
    function insert(array \$row)
    {
        \$columns = array_keys(\$row);
        \$values = array_values(\$row);
        \$sql = \"INSERT INTO {\$this->table_name} (\".implode(',',\$columns).\")\";
        array_fill(0, count(\$columns), '?');
        \$sql .= \"VALUES (\".implode(',',\$columns).\")\";
        \$temp = \$values;
        array_unshift(\$values, \$sql);
        \$status = call_user_func_array(array(\$this, 'db_link', 'query'), \$temp);
        if(\$status === false)
        {
            echo 'ERROR sql in inserter \\r\\n';
            return false;
        }
        elseif (\$status === null)
        {
            return false;
        }
        else
        {
            return true;
        }
    }\n";
        return $code;
    }
/************************************************namespace*************************************************************/
    function generate_namespace($database)
    {
        $code = "
namespace $database;
use mysqli;\n";
        return $code;
    }
/************************************************database_class********************************************************/
    function generate_database_class($database, $tables)
    {
        $code = "";
        $code .= $this->generate_database_init($database);
        foreach ($tables as $table)
        {
            $code .= $this->generate_database_table_function($table);
        }
        $code .= "}";
        return $code;
    }

    function generate_database_init($database)
    {
        $code = "
class $database
{
    const DATABASE = '{$database}';
    private \$db_link = null;
    
    function __construct(\$host, \$username, \$password, \$database, \$port=3306)
    {
        \$this->db_link = new Database(\$host, \$username, \$password, \$database, \$port);
    }\n";
        return $code;
    }

    function generate_database_table_function($table)
    {
        $code = "
    function {$table}()
    {
        return new table_manager(\$this->db_link, '{$table}', {$table}_row::class);
    }\n";
        return $code;
    }
/**********************************************query_class*************************************************************/

    function generate_query_class()
    {
        $query_class = "
class Database
{
    private \$link=null;
    private \$host = null;
    private \$database = null;
    private \$username = null;
    private \$password = null;
    private \$port = null;
    private \$errno = 0;
    private \$error = \"\";

    function __construct(\$host, \$username, \$password, \$database, \$port = 3306) {
        \$this->host = \$host;
        \$this->database = \$database;
        \$this->username = \$username;
        \$this->password = \$password;
        \$this->port = \$port;
        \$this->link = new mysqli(\$this->host,\$this->username,\$this->password, \$this->database, \$this->port);
        if(\$this->link === false)
        {
            \$this->error =  \$this->link->connect_error;
        }
    }

    function error()
    {
        return \$this->error;
    }
    function errno()
    {
        return \$this->errno;
    }
    private function set_error(int \$errno, string \$error)
    {
        \$this->errno = \$errno;
        \$this->error = \$error;
    }
    private function unset_error()
    {
        \$this->set_error(0, \"\");
    }

    function query(string \$str, ...\$args)
    {
        \$this->unset_error();
        \$result = null;
        \$types = \"\";
        for (\$i = 0; \$i < count(\$args); \$i++)
        {
            switch(gettype(\$args[\$i]))
            {
                case \"boolean\":
                    \$types .= \"s\";
                    break;
                case \"integer\":
                    \$types .= \"i\";
                    break;
                case \"double\":
                    \$types .= \"d\";
                    break;
                case \"float\":
                    \$types .= \"d\";
                    break;
                case \"string\":
                    \$types .= \"s\";
                    break;
                case \"NULL\":
                    \$types .= \"s\";
                    break;
                default:
                    \$types .= \"s\";
                    break;
            }
        }
        array_unshift(\$args, \$types);
        if (\$stmt = \$this->link->prepare(\$str))
        {
            // Bind variables to the prepared statement as parameters
            call_user_func_array(array(\$stmt, \"bind_param\"), \$args);

            // Attempt to execute the prepared statement
            if (\$stmt->execute())
            {
                /* Get the result */
                \$result_status = \$stmt->get_result();

                if (\$result_status == false)
                {
                    \$this->set_error(\$stmt->errno, \$stmt->error);
                    return false;
                }
                else
                {
                    \$result = [];
                    while (\$row = \$result_status->fetch_assoc())
                    {
                        array_push(\$result, \$row);
                    }
                }
            }
            else
            {
                \$this->set_error(\$stmt->errno, \$stmt->error);
                return false;
            }
            /* free results */
            \$stmt->free_result();
            // Close statement
            \$stmt->close();
        }
        else
        {
            \$this->set_error(\$stmt->errno, \$stmt->error);
            return false;
        }
        return \$result;
    }
}
";
        return $query_class;
    }


}
/**********************************************************************************************************************/
class Database
{
    private $link=null;
    private $host = null;
    private $database = null;
    private $username = null;
    private $password = null;
    private $port = null;
    private $errno = 0;
    private $error = "";

    function __construct($host, $username, $password, $database, $port = 3306) {
        $this->host = $host;
        $this->database = $database;
        $this->username = $username;
        $this->password = $password;
        $this->port = $port;
        $this->link = new mysqli($this->host,$this->username,$this->password, $this->database, $this->port);
        if($this->link === false)
        {
            $this->error =  $this->link->connect_error;
        }
    }

    function error()
    {
        return $this->error;
    }
    function errno()
    {
        return $this->errno;
    }
    private function set_error(int $errno, string $error)
    {
        $this->errno = $errno;
        $this->error = $error;
    }
    private function unset_error()
    {
        $this->set_error(0, "");
    }

    function query(string $str, ...$args)
    {
        $this->unset_error();
        $result = null;
        $types = "";
        for ($i = 0; $i < count($args); $i++)
        {
            switch(gettype($args[$i]))
            {
                case "boolean":
                    $types .= "s";
                    break;
                case "integer":
                    $types .= "i";
                    break;
                case "double":
                    $types .= "d";
                    break;
                case "float":
                    $types .= "d";
                    break;
                case "string":
                    $types .= "s";
                    break;
                case "NULL":
                    $types .= "s";
                    break;
                default:
                    $types .= "s";
                    break;
            }
        }
        array_unshift($args, $types);
        if ($stmt = $this->link->prepare($str))
        {
            // Bind variables to the prepared statement as parameters
            call_user_func_array(array($stmt, "bind_param"), $args);

            // Attempt to execute the prepared statement
            if ($stmt->execute())
            {
                /* Get the result */
                $result_status = $stmt->get_result();

                if ($result_status == false)
                {
                    $this->set_error($stmt->errno, $stmt->error);
                    return false;
                }
                else
                {
                    $result = [];
                    while ($row = $result_status->fetch_assoc())
                    {
                        array_push($result, $row);
                    }
                }
            }
            else
            {
                $this->set_error($stmt->errno, $stmt->error);
                return false;
            }
            /* free results */
            $stmt->free_result();
            // Close statement
            $stmt->close();
        }
        else
        {
            $this->set_error($stmt->errno, $stmt->error);
            return false;
        }
        return $result;
    }
}

/*********************************************************************************************************************/
define('DB_SERVER',   'localhost');
define('DB_USERNAME', 'root');
define('DB_PASSWORD', 'a');
define('DB_DATABASE', 'REGISTRO');

$generator = new db2php(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_DATABASE);
//echo $generator->generate();
$code = "<?php\n".$generator->generate();

$myfile = fopen(DB_DATABASE.".class.inc.php", "w") or die("Unable to open file!");
fwrite($myfile, $code);
fclose($myfile);

require_once "REGISTRO.class.inc.php";

$registro = new REGISTRO\REGISTRO(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_DATABASE);
$rows = $registro->User()->get_all();
echo count($rows);

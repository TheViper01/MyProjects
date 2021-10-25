#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<ctype.h>
#include <time.h>
#include<conio.h>

#ifndef MIN(X, Y)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif // !MIN

#ifndef MAX(X, Y)
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif // !MAX

/****************************************************************************************************************************************/

static void swap_g(void* a, void* b, size_t size_m)
{
	size_t temp, written = 0;
	while (MIN(sizeof(size_t), size_m - written) > 0)
	{
		temp = *(size_t*)(((char*)a) + written);
		*(size_t*)(((char*)a) + written) = *(size_t*)(((char*)b) + written);
		*(size_t*)(((char*)b) + written) = temp;
		written += MIN(sizeof(size_t), size_m - written);
	}
}


/****************************************************************************************************************************************/
enum bool {false=0, true=1};
enum value { ace = 1, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king };
enum color { black = 0, red };
enum suit { clubs = 0, diamond, hearts, spades };
typedef unsigned char BYTE;

struct card
{
	enum value value;
	enum color color;
	enum suit suit;

	//BYTE value; //1 - 13, Ace - King
	//BYTE color; //0=black, 1=red
	//BYTE suit;	//0=clubs, 1=diamond, 2=hearts, 3=spades
};

typedef struct deck
{
	struct card cards[52 * 8];  // the fonal dek has 8 normal deks, 52*8 cards
	size_t num_cards;
};

typedef struct player
{

	size_t id; // dealer in id 0
	enum bool stay;
	struct card cards[10];
	size_t num_cards;
	double balance;
	double bet_ammount;
};

//typedef struct bet
//{
//	struct player* player;
//	double ammount;
//};

typedef struct table
{
	size_t player_id_count;
	struct deck deck;
	struct player dealer; // dealer in id 0
	struct player players[7];
	size_t num_players;
	//struct bet bets[7*10];
	//size_t num_bets;
};

/****************************************************************************************************************************************/
static void shuffle_deck(struct deck* _deck)
{
	size_t i, j, k;
	struct card temp;
	srand(time(NULL));
	if (_deck->num_cards > 1)
	{
		for (i = 0; i < _deck->num_cards - 1; i++)
		{
			j = i + rand() / (RAND_MAX / (_deck->num_cards - i) + 1);
			//printf("Card value init: %d\n", _deck->cards[i].value);
			swap_g(&_deck->cards[j], &_deck->cards[i], sizeof(struct card));
		}
	}
	//for (size_t i = 0; i < _deck->num_cards; i++)
	//{
	//	printf("%d\n", _deck->cards[i].value);
	//}
	//while (true)
	//{

	//}
}


static void init_deck(struct deck* _deck)
{
	size_t i, j, k, color, arr_count = 0;
	_deck->num_cards = 52 * 8;
	for (k = 0; k < 8; k++)
	{
		for (i = 1; i <= 13; i++)
		{
			color = 1;
			for (j = 0; j < 4; j++)
			{
				_deck->cards[arr_count].value = i;
				_deck->cards[arr_count].color = color;
				_deck->cards[arr_count].suit = j;
				color++;
				if (color == 2)
				{
					color = 0;
				}
				arr_count++;
			}
		}
	}

	shuffle_deck(_deck);
}

static struct card pick_card(struct deck* _deck)
{
	struct card card_temp;
	_deck->num_cards--;
	card_temp = _deck->cards[_deck->num_cards];
	return card_temp;
}
/****************************************************************************************************************************************/

static void init_player(struct player* _player, size_t _id, double _money)
{
	_player->id = _id;
	_player->stay = false;
	_player->balance = _money;
	_player->num_cards = 0;
	_player->bet_ammount = 0;
}

static double make_bet(struct player* _player, double _money)
{
	
	_player->bet_ammount = MIN(_player->balance, _money);
	_player->balance -= _player->bet_ammount;
	//printf("balance: %lf   money: %lf    bet: %lf\n\n", _player->balance, _money, _player->bet_ammount);
	return _player->bet_ammount;
}

static struct card* player_pick_card(struct player* _player, struct table* _table)
{
	_player->cards[_player->num_cards] = pick_card(&_table->deck);
	_player->num_cards++;
	return &_player->cards[_player->num_cards - 1];
}

static void add_player(struct table* _table, double _money)
{
	init_player(&_table->players[_table->num_players], _table->player_id_count, _money);
	_table->num_players++;
	_table->player_id_count++;
}

static void init_table(struct table* _table)
{
	_table->player_id_count = 1;
	_table->num_players = 0;
	init_deck(&_table->deck);
	init_player(&_table->dealer, 0, 0.00);
}

static int stayng_players(struct table* _table)
{
	size_t i;
	int count = 0;
	for (i = 0; i < _table->num_players; i++)
	{
		if (_table->players[i].stay == true /* && hand_points(&_table->players[i]) <= 21*/) count++;
	}
	return count;
}

static int pending_players(struct table* _table)
{
	size_t i;
	int count = 0;
	for (i = 0; i < _table->num_players; i++)
	{
		if (_table->players[i].stay == false && _table->players[i].bet_ammount > 0.00) count++;
	}
	return count;
}

static void print_player_info(struct player* _player)
{
	size_t i;
	printf("*************************Player %d **************************\n", _player->id);
	printf("ID: %u\n", _player->id);
	printf("Balance: %lf\n", _player->balance);
	printf("Bet ammount: %lf\n", _player->bet_ammount);
	printf("Num cards: %u\n", _player->num_cards);
	for (i = 0; i < _player->num_cards; i++)
	{
		switch (_player->cards[i].value)
		{
		case ace:
			printf("Ace");
			break;
		case jack:
			printf("Jack");
			break;
		case queen:
			printf("Queen");
			break;
		case king:
			printf("King");
			break;
		default:
			printf("%d", _player->cards[i].value);
			break;
		}
		printf("\t");
		switch (_player->cards[i].suit)
		{
		case clubs:
			printf("clubs");
			break;
		case diamond:
			printf("diamond");
			break;
		case hearts:
			printf("hearts");
			break;
		case spades:
			printf("spades");
			break;
		}
		printf("\t");
		switch (_player->cards[i].color)
		{
		case black:
			printf("black");
			break;
		case red:
			printf("red");
			break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Hand points:	%d\n", hand_points(_player));
}

static void print_dealer_info(struct player* _player)
{
	size_t i;
	printf("*************************Dealer**************************\n");
	printf("Num cards: %u\n", _player->num_cards);
	for (i = 0; i < _player->num_cards; i++)
	{
		switch (_player->cards[i].value)
		{
		case ace:
			printf("Ace");
			break;
		case jack:
			printf("Jack");
			break;
		case queen:
			printf("Queen");
			break;
		case king:
			printf("King");
			break;
		default:
			printf("%d", _player->cards[i].value);
			break;
		}
		printf("\t");
		switch (_player->cards[i].suit)
		{
		case clubs:
			printf("clubs");
			break;
		case diamond:
			printf("diamond");
			break;
		case hearts:
			printf("hearts");
			break;
		case spades:
			printf("spades");
			break;
		}
		printf("\t");
		switch (_player->cards[i].color)
		{
		case black:
			printf("black");
			break;
		case red:
			printf("red");
			break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Hand points:	%d\n", hand_points(_player));
}

static void print_all_players_info(struct table* _table)
{
	size_t i;
	system("Cls");
	printf("-------------------------------Players info-----------------------------\n");
	print_dealer_info(&_table->dealer);
	for (i = 0; i < _table->num_players; i++)
	{
		print_player_info(&_table->players[i]);
		won_lost_routine(&_table->players[i], _table);
	}
	printf("-----------------------------------------------------------------------\n\n");
}
/****************************************************************************************************************************************/
static int hand_points(struct player* _player)
{
	size_t i, aces = 0;
	int points = 0, points_to_21 = 0;
	for (i = 0; i < _player->num_cards; i++)
	{
		if (_player->cards[i].value == ace)
		{
			aces++;
		}
		else if (_player->cards[i].value >= 11 && _player->cards[i].value <= 13)
		{
			points += 10;
		}
		else
		{
			points += _player->cards[i].value;
		}
	}
	points_to_21 = 21 - points;
	if (aces > 0)
	{
		if ((11 + aces - 1) <= points_to_21)
		{
			points += 11 + aces - 1;
		}
		else
		{
			points += aces;
		}
	}
	return points;
}

static struct palyer* winner(struct palyer* _player_1, struct palyer* _player_2)
{
	int player_1_points = hand_points(_player_1);
	int player_2_points = hand_points(_player_2);

	if (player_1_points == player_2_points || player_1_points > 21 && player_2_points > 21)
	{
		return 0;
	}
	else if (player_1_points > 21)
	{
		return _player_2;
	}
	else if (player_2_points > 21)
	{
		return _player_1;
	}
	else if (player_1_points > player_2_points)
	{
		return _player_1;
	}
	else
	{
		return _player_2;
	}
}

static int won_lost(struct player* _player, struct table* _table) //0=lost, 1=won, 2=tie, -1=still in the game
{
	int player_points = hand_points(_player);
	int dealer_points = hand_points(&_table->dealer);
	enum bool player_stay = _player->stay;
	enum bool dealer_stay = _table->dealer.stay;
	if (player_points == 21)
	{
		_player->stay = true;
		return 1;
	}
	else if (player_points > 21)
	{
		_player->stay = true;
		return 0;
	}
	else if (dealer_points > 21)
	{
		_table->dealer.stay = true;
		_player->stay = true;
		return 1;
	}
	else if (player_stay == true && dealer_stay == true)
	{
		if (player_points > dealer_points)
		{
			return 1;
		}
		else if (dealer_points > player_points)
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return -1;
	}
}

static double won_lost_ammount(struct player* _player, struct table* _table)
{
	int player_points = hand_points(_player);
	int dealer_points = hand_points(&_table->dealer);
	enum bool player_stay = _player->stay;
	enum bool dealer_stay = _table->dealer.stay;
	if (player_points == 21)
	{
		_player->stay = true;
		return _player->bet_ammount * 2 / 3;
	}
	else if (player_points > 21)
	{
		_player->stay = true;
		return -_player->bet_ammount;
	}
	else if (dealer_points > 21)
	{
		_table->dealer.stay = true;
		return _player->bet_ammount;
	}
	else if (player_stay == dealer_stay == true)
	{
		if (player_points > dealer_points)
		{
			return _player->bet_ammount;
		}
		else if (dealer_points > player_points)
		{
			return -_player->bet_ammount;
		}
		else
		{
			return 0.00;
		}
	}
	else
	{
		return 0.00;
	}
}
static int won_lost_routine(struct player* _player, struct table* _table)
{
	//print_player_info(_player);
	switch (won_lost(_player, _table))
	{
	case -1:
		return 0;
	case 0:
		printf("        YOU LOST %lf\n\n", -won_lost_ammount(_player, _table));
		//_player->bet_ammount = 0.00;
		return 1;
	case 1:
		printf("        YOU WON %lf$\n\n", won_lost_ammount(_player, _table));
		_player->balance += _player->bet_ammount + won_lost_ammount(_player, _table);
		//_player->bet_ammount = 0.00;
		return 1;
	case 2:
		printf("        IT'S A TIE\n\n");
		_player->balance += _player->bet_ammount;
		//_player->bet_ammount = 0.00;
		return 1;
	}
}
/****************************************************************************************************************************************/
static void give_first_2_cards(struct table* _table)
{
	size_t i;
	struct card* temp_card;
	shuffle_deck(&_table->deck);
	//print_all_players_info(_table);
	system("Cls");
	printf("////////////////////////////////////First 2 cars/////////////////////////////////////////////////\n");
	temp_card = player_pick_card(&_table->dealer, _table); //dealer card
	print_dealer_info(&_table->dealer);
	for (i = 0; i < _table->num_players; i++)	//Piking first card stage
	{
		if (_table->players[i].bet_ammount == 0.00) continue;
		temp_card = player_pick_card(&_table->players[i], _table);
		temp_card = player_pick_card(&_table->players[i], _table);
		print_player_info(&_table->players[i]);;
		won_lost_routine(&_table->players[i], _table);
	}
	printf("////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
}

static void give_more_cards(struct table* _table)
{
	size_t j;
	char char_temp_1;
	struct card* temp_card;
	while (pending_players(_table) > 0)
	{
		for (j = 0; j < _table->num_players; j++)
		{
			if (_table->players[j].bet_ammount == 0.00 || _table->players[j].stay == true || won_lost(&_table->players[j], _table) == 0) continue;
			print_player_info(&_table->players[j]);
			while (1)
			{
				printf("Player number %d pick a card (Y/N): ", _table->players[j]);
				scanf("%c", &char_temp_1); getchar();

				if (char_temp_1 == 'y' || char_temp_1 == 'Y')
				{
					temp_card = player_pick_card(&_table->players[j], _table);
					break;
				}
				else if (char_temp_1 == 'n' || char_temp_1 == 'N')
				{
					_table->players[j].stay = true;
					break;
				}
				else
				{
					printf("Invalid option!\n");
				}
			}
			won_lost(&_table->players[j], _table);
			print_all_players_info(_table);
		}
	}
}

static int dealer_pick_until(struct table* _table) //return dealer hand points
{
	while (hand_points(&_table->dealer) < 17)
	{
		player_pick_card(&_table->dealer, _table);
	}
	_table->dealer.stay = true;
	return hand_points(&_table->dealer);
}

static void new_round(struct table* _table)
{
	size_t i;
	init_player(&_table->dealer, _table->dealer.id, _table->dealer.balance);
	for (i = 0; i < _table->num_players; i++)
	{
		init_player(&_table->players[i], _table->players[i].id, _table->players[i].balance);
	}
}
/****************************************************************************************************************************************/

int main()
{
	size_t i, j, uint_temp_1;
	char char_temp_1;
	double default_money = 2000.00, temp_bet;
	struct table bj_table;
	struct card* temp_card;
	init_table(&bj_table);
	while (1) //main loop
	{
		printf("Insert number of players (max 7): ");
		scanf("%u", &uint_temp_1); getchar();
		for (i = 0; i < MIN(uint_temp_1, 7); i++)
		{
			add_player(&bj_table, default_money);
		}
		while (1) //rounds loop
		{
			for (i = 0; i < bj_table.num_players; i++)	//betting stage
			{
				print_all_players_info(&bj_table);
				print_player_info(&bj_table.players[i]);
				printf("Bet ammount: ");
				scanf("%lf", &temp_bet); getchar();
				make_bet(&bj_table.players[i], temp_bet);
			}
			give_first_2_cards(&bj_table);
			give_more_cards(&bj_table);
			dealer_pick_until(&bj_table);

			print_all_players_info(&bj_table);

			while (1)
			{
				printf("\n\nNew round? (Y/N): ");
				scanf("%c", &char_temp_1); getchar();

				if (char_temp_1 == 'y' || char_temp_1 == 'Y')
				{
					break;
				}
				else if (char_temp_1 == 'n' || char_temp_1 == 'N')
				{
					return 0;
					break;
				}
				else printf("invalid option!\n");
			}
			printf("\n\nNew round? (Y/N)\n");
			new_round(&bj_table);
		}
	}
	return 0;
}

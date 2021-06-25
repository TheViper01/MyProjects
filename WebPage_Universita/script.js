/***********************scripts to load afrer the page is loaded*****************/
window.onload = function() {
    AccordionListner();
}
/***********************************START-NavigationBar**************************/
/* Toggle between adding and removing the "responsive" class to topnav when the user clicks on the icon */
function NavBarDropdown() {
    var x = document.getElementById("myTopnav");
    if (x.className === "topnav") {
        x.className += " responsive";
    } else {
        x.className = "topnav";
    }
}
/***********************************END-NavigationBar****************************/
/***********************************START-accordion******************************/
function AccordionListner(cls="accordion") {
    var acc = document.getElementsByClassName("accordion");
    var body = document.getElementsByTagName("BODY")[0];
    console.log(body.scrollHeight);
    var i;

    for (i = 0; i < acc.length; i++) {
        acc[i].addEventListener("click", function() {
            this.classList.toggle("accordion_active");
            var panel = this.nextElementSibling;
            if (panel.style.maxHeight) {
                body.style.maxHeight = body.style.scrollHeight - panel.scrollHeight + "px";
                panel.style.maxHeight = null;
            } else {
                body.style.maxHeight = body.style.scrollHeight + panel.scrollHeight + "px";
                panel.style.maxHeight = panel.scrollHeight + "px";
            }
        });
    }
}
/***********************************END-accordion********************************/

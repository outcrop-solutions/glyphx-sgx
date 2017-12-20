$( document ).ready(function() {
    $("#buyNow").addClass("disabled");
    $("#createButton").addClass("disabled");
});

function stopRKey(evt) { 
  var evt = (evt) ? evt : ((event) ? event : null); 
  var node = (evt.target) ? evt.target : ((evt.srcElement) ? evt.srcElement : null); 
  if (evt.keyCode == 13)  {return false;} 
} 

document.onkeypress = stopRKey; 


// ------------------------ USER NAME INPUT HANDLERS ------------------------
$("#user_name").on("blur", function() {
    if ($("#user_name").val() !== "") {
        if ($("#user_name").val().split(" ").length > 1) {
            $("#user_name").removeClass("invalid");
            $("#user_name").addClass("valid");
        }
        else {
            $("#user_name").removeClass("valid");
            $("#user_name").addClass("invalid");
        }

        checkShouldEnableBuyNow();
    }
})

$('#user_name').keypress(function(event){
	var keycode = (event.keyCode ? event.keyCode : event.which);
	if(keycode == '13'){
		$("#user_name").blur();
	}
});


// ------------------------ USER EMAIL INPUT HANDLERS ------------------------
$("#user_email").on("blur", function() {
    if ($("#user_email").val() !== "") {
        if ($("#user_PROMO").val() !== "") {
            //check is promo code exists and update page
            $("#itemDiscount").css('display', 'none');
            $("#promoLoader").css('display', '');

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    // Action to be performed when the document is read;
                    if (this.responseText) {

                        var response = JSON.parse(this.responseText);
                        $("#itemDiscount").text("-$" + response.discount.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                        $("#itemTotal").text("$" + response.total.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                        $("#itemDiscount").css('display', '');
                        $("#promoLoader").css('display', 'none');

                        if ($("#user_email").val() != "") {
                            var re = new RegExp(response.emailRegEx, "g");
                            var reMatch = $("#user_email").val().match(re);
                            if (reMatch == null) {
                                $("#itemDiscount").text("-$0");
                                $("#itemTotal").text("$" + response.original.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",") );
                                alert("That PROMO code does not apply to the email selected.");
                            }
                        }
                    }
                    else {
                        $("#itemDiscount").text("-$0");
                        $("#itemTotal").text("$3,525");
                        $("#itemDiscount").css('display', '');
                        $("#promoLoader").css('display', 'none');
                    }
                }
            };
            xhttp.open("GET", "/queryPromoCode?promoCode=" + $("#user_PROMO").val(), true);
            xhttp.send();
        }
        else {
            $("#itemDiscount").text("");
            $("#itemTotal").text("$3,525");
        }

        checkShouldEnableBuyNow();
    }
})



$('#user_email').keypress(function(event){
	var keycode = (event.keyCode ? event.keyCode : event.which);
	if(keycode == '13'){
		$("#user_email").blur();
	}
});


// ------------------------ USER PASSWORD INPUT HANDLERS ------------------------
$("#user_password").on("blur", function() {
    $(".container").css('display', 'none');

    if ($("#user_password").val() !== "") {
        if ($("#user_password").val().length > 7) {
            $("#user_password").removeClass("invalid");
            $("#user_password").addClass("valid");
        }
        else {
            $("#user_password").removeClass("valid");
            $("#user_password").addClass("invalid");
        }

        if ($('#five').is(':checked') || $('#twentyfive').is(':checked')) { 
            $("#user_password").removeClass("valid");
            $("#user_password").addClass("invalid");
        }

        if ($("#user_confirmpassword").val() !== "") {
            if ($("#user_password").val() === $("#user_confirmpassword").val()) {
                $("#user_confirmpassword").removeClass("invalid");
                $("#user_confirmpassword").addClass("valid");
            }
            else {
                $("#user_confirmpassword").removeClass("valid");
                $("#user_confirmpassword").addClass("invalid");
            }
        }

        checkShouldEnableBuyNow();
    }
})


$("#user_password").on("focus", function() {
    $(".container").css('display', '');
})


$("#user_password").on("input", function() {
    var score = scorePassword($("#user_password").val());

    if (score >= 100) {
        $("#onehundred").prop("checked", true);
    }
    else if (score >= 75) {
        $("#seventyfive").prop("checked", true);
    }
    else if (score >= 50) {
        $("#fifty").prop("checked", true);
    }
    else if (score >= 25) {
        $("#twentyfive").prop("checked", true);
    }
    else {
        $("#five").prop("checked", true);
    }
})


$('#user_password').keypress(function(event){
	var keycode = (event.keyCode ? event.keyCode : event.which);
	if(keycode == '13'){
		$("#user_password").blur();
	}
});


$("#user_confirmpassword").on("blur", function() {
    if ($("#user_confirmpassword").val() !== "") {
        if ($("#user_password").val() === $("#user_confirmpassword").val()) {
            $("#user_confirmpassword").removeClass("invalid");
            $("#user_confirmpassword").addClass("valid");
        }
        else {
            $("#user_confirmpassword").removeClass("valid");
            $("#user_confirmpassword").addClass("invalid");
        }

        checkShouldEnableBuyNow();
    }
})

$('#user_confirmpassword').keypress(function(event){
	var keycode = (event.keyCode ? event.keyCode : event.which);
	if(keycode == '13'){
		$("#user_confirmpassword").blur();
	}
});


// ------------------------ PROMO CODE INPUT HANDLERS ------------------------

$("#user_PROMO").on("blur", function() {
    if ($("#user_PROMO").val() !== "") {
        //check is promo code exists and update page
        $("#itemDiscount").css('display', 'none');
        $("#promoLoader").css('display', '');


        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                // Action to be performed when the document is read;
                if (this.responseText) {

                    var response = JSON.parse(this.responseText);
                    if ($("#user_email").val() != "") {
                        var re = new RegExp(response.emailRegEx, "g");
                        var reMatch = $("#user_email").val().match(re);
                        if (reMatch == null) {
                            $("#itemDiscount").text("-$0");
                            $("#itemTotal").text("$" + response.original.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",") );
                            alert("That PROMO code does not apply to the email selected.");
                            $("#itemDiscount").css('display', '');
                            $("#promoLoader").css('display', 'none');
                        }
                        else {
                            $("#itemDiscount").text("-$" + response.discount.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                            $("#itemTotal").text("$" + response.total.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                            $("#itemDiscount").css('display', '');
                            $("#promoLoader").css('display', 'none');
                        }
                    }
                    else {
                        $("#itemDiscount").text("-$" + response.discount.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                        $("#itemTotal").text("$" + response.total.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ","));
                        $("#itemDiscount").css('display', '');
                        $("#promoLoader").css('display', 'none');
                    }

                    if (response.total == 0) {
                        $("#buyNowDiv").css('display', 'none');
                        $("#createButtonDiv").css('display', '');
                    }
                    else {
                        $("#buyNowDiv").css('display', '');
                        $("#createButtonDiv").css('display', 'none');
                    }
                }
                else {
                    $("#itemDiscount").text("-$0");
                    $("#itemTotal").text("$3,525");
                    $("#itemDiscount").css('display', '');
                    $("#promoLoader").css('display', 'none');
                    $("#buyNowDiv").css('display', '');
                    $("#createButtonDiv").css('display', 'none');
                }
            }
        };
        xhttp.open("GET", "/queryPromoCode?promoCode=" + $("#user_PROMO").val(), true);
        xhttp.send();


        
    }
    else {
        $("#itemDiscount").text("");
        $("#itemTotal").text("$3,525");
    }
})


$('#user_PROMO').keypress(function(event){
	var keycode = (event.keyCode ? event.keyCode : event.which);
	if(keycode == '13'){
		$("#user_PROMO").blur();
	}
});


$("#createButton").click(function() {
        /*
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                // Action to be performed when the document is read;
                debugger;
            }
        };
        xhttp.open("GET", "/freePromoCode?promoCode=" + $("#user_PROMO").val(), true);
        xhttp.send();
        */

        /*
        $.post("/freePromoCode",
        {
            userInfo: "Donald Duck",
        },
        function(data, status){
            alert("Data: " + data + "\nStatus: " + status);
        });
        

        $.get("/download", function(data, status){
            data;
            status;
            debugger;
            window.location.href = data;
        });

        */

        var userObj = {
            name: $("#user_name").val(),
            email: $("#user_email").val(),
            password: $("#user_password").val(),
            PROMO: $("#user_PROMO").val(),
        }

        $.get("/freePromoCode?userInfo=" + JSON.stringify(userObj), function(data, status){
            window.location.href = data;
        });

});


// ------------------------ FUNCTIONS ------------------------

function checkShouldEnableBuyNow() {
    if ($("#user_name").hasClass("valid") && $("#user_email").hasClass("valid") && $("#user_password").hasClass("valid") && $("#user_confirmpassword").hasClass("valid")) {
        $("#buyNow").removeClass("disabled");
        $("#createButton").removeClass("disabled");
        $("#disabledMessage").css('display', 'none');
        return true;
    }
    else {
        $("#buyNow").addClass("disabled");
        $("#createButton").addClass("disabled");
        $("#disabledMessage").css('display', '');
        return false;
    }
    
}

function scorePassword(pass) {
    var score = 0;

    if (!pass) {
        return score;
    }

    // award every unique letter until 5 repetitions
    var letters = new Object();
    for (var i=0; i<pass.length; i++) {
        letters[pass[i]] = (letters[pass[i]] || 0) + 1;
        score += 5.0 / letters[pass[i]];
    }

    // bonus points for mixing it up
    var variations = {
        digits: /\d/.test(pass),
        lower: /[a-z]/.test(pass),
        upper: /[A-Z]/.test(pass),
        nonWords: /\W/.test(pass),
    }

    variationCount = 0;

    for (var check in variations) {
        variationCount += (variations[check] == true) ? 1 : 0;
    }

    score += (variationCount - 1) * 10;

    return parseInt(score);
}
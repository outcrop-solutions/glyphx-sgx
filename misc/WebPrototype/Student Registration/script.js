$( document ).ready(function() {
    document.getElementById("buyNow").disabled = true;
});


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


$("#user_email").on("blur", function() {
    if ($("#user_email").val() !== "") {
        checkShouldEnableBuyNow();
    }
})


$("#user_password").on("blur", function() {
    //$(".container").css('display', 'none');

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


function checkShouldEnableBuyNow() {
    if ($("#user_name").hasClass("valid") && $("#user_email").hasClass("valid") && $("#user_password").hasClass("valid") && $("#user_confirmpassword").hasClass("valid")) {
        $('.overlay').css('display', 'none');
        document.getElementById("buyNow").disabled = false;
    }
    else {
        $('.overlay').css('display', '');
        document.getElementById("buyNow").disabled = true;
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
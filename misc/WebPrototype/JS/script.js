
 $(document).ready(function() {
    //$('#tableSelect').material_select();
    $('select').material_select();
  });
  
  
  //FUNCTIONS

  function toggleNav() {
    if (document.getElementById("filterNav").style.width == "400px") {
        document.getElementById("filterNav").style.width = "0";
    }
    else {
        document.getElementById("filterNav").style.width = "400px";
    }
     $('select').material_select();
}

function toggleFilter() {
    if ($('#filterViewButton').hasClass("fa-caret-up")) {
        $("#filterView").toggle(); 
        $('#filterViewButton').removeClass("fa-caret-up");
        $('#filterViewButton').addClass("fa-caret-down");
    }
    else {
        $("#filterView").toggle(); 
        $('#filterViewButton').removeClass("fa-caret-down");
        $('#filterViewButton').addClass("fa-caret-up");
    }
}
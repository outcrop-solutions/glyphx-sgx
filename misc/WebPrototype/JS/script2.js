function init() {
    console.log("called init");
    $('.tap-target').tapTarget('open');


    $('i.close').on('click', function(e){
      onCancelFilter(e);
    });

    
    $('#clearAllFilters').on('click', function(e){
      onClearAllFilters(e);
    });

    $('#toggleFilters').on('click', function(e){
      onToggleFilters(e);
    });
}

function onCancelFilter(e){
  alert("onCancelFilter!");
}

function onClearAllFilters(e){
  alert("onClearAllFilters!");
}


function onToggleFilters(e){
  //alert("onToggleFilters!");
  if($("#toggleFilters").html().split("<")[0] == "Hide")
  $("#toggleFilters").html("Show" + "<" + $("#toggleFilters").html().split("<")[1]);
  else
  $("#toggleFilters").html("Hide"  + "<" + $("#toggleFilters").html().split("<")[1]);
  
}
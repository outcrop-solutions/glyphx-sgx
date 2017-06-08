function init() {
    console.log("called init");
    $('.material-icons .close').on('chip.delete', function(e, chip){
    alert("HEy!");
  });
}
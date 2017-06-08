
 $(document).ready(function() {
    
    // Prepares the select boxes (materialize)
    $('select').material_select();

    // Creating an alertify error message
    if(!alertify.showError){
        alertify.dialog('showError', function factory(){
            return{
                build: function () {
                    var htmlWindow = '<i class="fa fa-times-circle fa-2x" style="vertical-align:middle; color:#881e22;" </i>'
                    htmlWindow += 'Input out of range';
                    this.setHeader(htmlWindow);
                }
            }
        }, false, 'alert');
    }

    // Creating new slider
    var slider = document.getElementById('range');
    noUiSlider.create(slider, {
        start: [0, 100],
        connect: true,
        step: 1,
        range: {
            'min': 0,
            'max': 100
        },
        format: wNumb({
            decimals: 0
        })
    });

    // Updates text field values when slider is dragged
    slider.noUiSlider.on('slide', function () {
        data = slider.noUiSlider.get();

        $('#min').val(data[0]);
        $('#max').val(data[1]);
    });

    $('#min').change(function () {
        if (parseInt($('#min').val()) > parseInt($('#max').val())) {
            $('#min').val($('#max').val())
        }
    });

    $('#max').change(function () {
        if (parseInt($('#max').val()) < parseInt($('#min').val())) {
            $('#max').val($('#min').val())
        }
    });

    // Listens to any change on the min text field
    $('#min').each(function () {
        var elem = $(this);

        // Save current value of element
        elem.data('oldVal', elem.val());

        // Look for changes in the value
        elem.bind("propertychange change click keyup input paste", function (event) {
            //if enter was pressed run the button's function
            if (event.which == 13) {
                event.preventDefault();
                registerFunc();
            }
            else {
                // If value has changed...
                if (elem.data('oldVal') != elem.val()) {
                    // Updated stored value
                    elem.data('oldVal', elem.val());

                    // Grab slider data [min,max]
                    data = slider.noUiSlider.get();

                    if (parseInt($('#min').val()) < 0) {
                        alertify.showError("The input you chose was out of the range.");
                        slider.noUiSlider.set([0, data[1]]);
                        $('#min').val('0');
                    }
                    else if (parseInt($('#min').val()) > 100) {
                        alertify.showError("The input you chose was out of the range.");
                        slider.noUiSlider.set([100, 100]);
                        $('#min').val('100');
                    }
                    else if (parseInt($('#min').val()) > parseInt(data[1])) {
                        slider.noUiSlider.set([data[1], data[1]]);
                        //$('#min').val($('#max').val());
                    }
                    else {
                        slider.noUiSlider.set([$('#min').val(), data[1]]);
                    }
                    
                    
                }
            }

        });
    });

    // Listens to any change on the max text field
    $('#max').each(function () {
        var elem = $(this);

        // Save current value of element
        elem.data('oldVal', elem.val());

        // Look for changes in the value
        elem.bind("propertychange change click keyup input paste", function (event) {
            //if enter was pressed run the button's function
            if (event.which == 13) {
                event.preventDefault();
                registerFunc();
            }
            else {
                // If value has changed...
                if (elem.data('oldVal') != elem.val()) {
                    // Updated stored value
                    elem.data('oldVal', elem.val());

                    // Do action
                    data = slider.noUiSlider.get();

                    if (parseInt($('#max').val()) > 100) {
                        alertify.showError("The input you chose was out of the range.");
                        slider.noUiSlider.set([data[0], 100]);
                        $('#max').val("100");
                    }
                    else if (parseInt($('#max').val()) < 0) {
                        alertify.showError("The input you chose was out of the range.");
                        slider.noUiSlider.set([0, 0]);
                        $('#max').val('0');
                    }
                    else if (parseInt($('#max').val()) < parseInt(data[0])) {
                        slider.noUiSlider.set([data[0], data[0]]);
                        //$('#max').val($('#min').val());
                    }
                    else {
                        slider.noUiSlider.set([data[0], $('#max').val()]);
                    }
                    
                }
            }
        });
    });
  });
  
  
  //FUNCTIONS

//Toggles view of the filter side bar
function toggleNav() {
    if (document.getElementById("filterNav").style.width == "450px") {
        document.getElementById("filterNav").style.width = "0";
        $('#launchFilter').css({"backgroundColor": "transparent"});
    }
    else {
        $('#launchFilter').css({"backgroundColor": "rgb(59, 113, 158)"});
        document.getElementById("filterNav").style.width = "450px";
    }
      /*$('.chips').material_chip();
  $('.chips-initial1').material_chip({
    data: [{
      tag: 'Apple',
    }, {
      tag: 'Microsoft',
    }, {
      tag: 'Google',
    }],
  });*/
    $('.material-icons .close').on('chip.delete', function(e, chip){
    alert("HEy!");
  });
}

//Toggles view of the top section of the filter side bar
function toggleFilter() {
    if ($('#filterViewButton').hasClass("fa-caret-up")) {
        $("#filterView").toggle(); 
        $("#tableSelect").toggle(); 
        $('#filterViewButton').removeClass("fa-caret-up");
        $('#filterViewButton').addClass("fa-caret-down");
    }
    else {
        $("#filterView").toggle();
        $("#tableSelect").toggle();  
        $('#filterViewButton').removeClass("fa-caret-down");
        $('#filterViewButton').addClass("fa-caret-up");
    }
}
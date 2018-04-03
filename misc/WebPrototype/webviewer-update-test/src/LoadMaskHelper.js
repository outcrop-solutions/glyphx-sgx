var splashScreenHidden = false;

export function hideSplashScreen() {
    if (splashScreenHidden) {
        return;
    }

    var lm = document.getElementById("ipl-progress-indicator");
    
    if (lm) {
        setTimeout(() => {
            (lm ? lm.classList.add('available') : null);
            setTimeout(() => {
                (lm ? (lm.parentElement ? lm.outerHTML = '' : null) : null);
            }, 2000)
        }, 1000)
    }
};


/**
 * This function shows the in app loadmask(cicular waiting).
 */
export function showLoadMask(){
    //document.getElementById("LoadMask").style.visibility = "visible";
};


/**
 * This function shows the in app loadmask(cicular waiting).
 */
export function hideLoadMask(){
    //document.getElementById("LoadMask").style.visibility = "visible";
};
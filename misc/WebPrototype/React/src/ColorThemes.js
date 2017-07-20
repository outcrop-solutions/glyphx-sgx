

// Will darken or lighten a color by a percent (-1.0 to 1.0) input
function shadeHexColors(color, percent) {   
    var f = parseInt(color.slice(1),16), t = percent < 0 ? 0 : 255, p = percent < 0 ? percent * -1 : percent, R = f >> 16, G = f >> 8 & 0x00FF, B = f & 0x0000FF;
    return "#" + (0x1000000 + (Math.round((t - R) * p) + R) * 0x10000 + (Math.round((t - G) * p) + G) * 0x100 + (Math.round((t - B) * p) + B)).toString(16).slice(1);
}


const themeSettingColors = [
    { /* SynGlyphX Theme */
        rangeColor: {
            sliderCircle: "#2d3091",
            sliderTrack: shadeHexColors("#2d3091", 0.4),
            textFieldUnderline: "#2d3091",
            toggleCircle: "#2d3091",
            toggleTrack: shadeHexColors("#2d3091", 0.4),
            selectedBackground: "#2d3091",
            selectedText: "#ffffff",
            text: "black",
            deleteHover: "#b81616",
            addHover: "#339cee"
        },

        elasticColor: {
            checkBox: "#2d3091",
            checkAllBox: shadeHexColors("#575d5e", -0.4),
            searchBoxUnderline: "#2d3091"
        },

        filterTabColor: {
            titleText: "#ffffff",
            tabBackground: shadeHexColors("#2d3091", -0.4),
        },

        collapsibleColor: {
            mainBackground: "#2d3091",
            mainCollapsed: shadeHexColors("#2d3091", -0.4),
            mainHover: shadeHexColors("#2d3091", 0.2),
            mainText: "#ffffff",
            mainIcon: "#ffffff",

            subBackground: "#575d5e",
            subCollapsed: "#575d5e",
            subHover: shadeHexColors("#2d3091", 0.2),
            subText: "#ffffff",
            pinned: "#ffffff",
            unpinned: shadeHexColors("#2d3091", -0.4)
        },

        pinFilterColor: {
            addPinBackground: shadeHexColors("#2d3091", -0.4),
            okButton: "#2d3091",
            cancelButton: "#575d5e",
            searchBoxUnderline: "#2d3091"
        },

        hideTopViewButtonColor: {
            background:  shadeHexColors("#575d5e", -0.4),
            icon: "#ffffff"
        },

        settingsModalColor: {
            saveButton: "#2d3091",
            cancelButton: "#575d5e",
            text: "black",
            selectedBackground: "#2d3091",
            selectedText: "#ffffff"
        },

        saveModalColor: {
            saveButton: "#2d3091",
            cancelButton: "#575d5e",
            textFieldUnderline: "#2d3091",
        },

        viewSelectColor: {
            text: "black",
            selectedBackground: "#2d3091",
            selectedText: "#ffffff"
        },

        tableSelectColor: {
            text: "black",
            selectedBackground: "#2d3091",
            selectedText: "#ffffff"
        },

        topNavbarColor: {
            barBackground: "#2d3091"
        },

        overviewButtonsColor: {
            background: "#2d3091",
            text: "white"
        },

        filterOverviewColor: {
            badgeBackground: shadeHexColors("#2d3091", -0.4),
            badgeText: "white",
            deleteHover: "#b81616",
            elasticHover: "#339cee",
            rangeHover: "#339cee"
        },
    },

    //----------------------------------------------------------------------------------------------

    { /* Gannon Theme */
        rangeColor: {
            sliderCircle: "#9d2235",
            sliderTrack: shadeHexColors("#9d2235", 0.4),
            textFieldUnderline: "#9d2235",
            toggleCircle: "#9d2235",
            toggleTrack: shadeHexColors("#9d2235", 0.4),
            selectedBackground: "#9d2235",
            selectedText: "#ffffff",
            text: "black",
            deleteHover: "#b81616",
            addHover: "#339cee"
        },

        elasticColor: {
            checkBox: "#9d2235",
            checkAllBox: "#f1b434",
            searchBoxUnderline: "#9d2235"
        },

        filterTabColor: {
            titleText: "#ffffff",
            tabBackground: shadeHexColors("#9d2235", -0.4),
        },

        collapsibleColor: {
            mainBackground: "#9d2235",
            mainCollapsed: shadeHexColors("#9d2235", -0.2),
            mainHover: shadeHexColors("#9d2235", 0.2),
            mainText: "#ffffff",
            mainIcon: "#ffffff",

            subBackground: "#f1b434",
            subCollapsed: "#f1b434",
            subHover: shadeHexColors("#9d2235", 0.2),
            subText: "#ffffff",
            pinned: "#ffffff",
            unpinned: shadeHexColors("#9d2235", -0.4)
        },

        pinFilterColor: {
            addPinBackground: shadeHexColors("#9d2235", -0.4),
            okButton: "#9d2235",
            cancelButton: "#f1b434",
            searchUnderline: "#2d3091"
        },

        hideTopViewButtonColor: {
            background: "#f1b434",
            icon: "#ffffff"
        },

        settingsModalColor: {
            saveButton: "#9d2235",
            cancelButton: "#f1b434",
            text: "black",
            selectedBackground: "#9d2235",
            selectedText: "#ffffff"
        },

        saveModalColor: {
            saveButton: "#9d2235",
            cancelButton: "#f1b434",
            textFieldUnderline: "#9d2235",
        },

        viewSelectColor: {
            text: "black",
            selectedBackground: "#9d2235",
            selectedText: "#ffffff"
        },

        tableSelectColor: {
            text: "black",
            selectedBackground: "#9d2235",
            selectedText: "#ffffff"
        },

        topNavbarColor: {
            barBackground: "#9d2235"
        },

        overviewButtonsColor: {
            background: "#9d2235",
            text: "white"
        },

        filterOverviewColor: {
            badgeBackground: shadeHexColors("#9d2235", -0.4),
            badgeText: "white",
            deleteHover: "#b81616",
            elasticHover: "#339cee",
            rangeHover: "#339cee"
        },
    },
]


export default themeSettingColors;
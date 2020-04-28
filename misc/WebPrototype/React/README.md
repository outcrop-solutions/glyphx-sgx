Table of Contents

[File Dictionary](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#file-dictionary-each-file-will-have-more-documentation-in-the-form-of-comments)<br>
[How to add a new viz to allowed list](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#how-to-add-a-new-viz-to-the-allowed-list)<br>
[How to make a new build and push to server](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#how-to-make-a-new-build-and-push-to-server)<br>
[How to hook up announcements to backend](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#how-to-hook-up-announcements-to-backend)<br>
[Implementation ideas for creating standalone vizs without a login](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#implementation-ideas-for-standalone-vizs)<br>
[ServerJS Node Module Configuration](https://github.com/SynGlyphX/sgx/tree/kevin/ui-remodel/misc/WebPrototype/React#microsoft-azure-active-directory-ad-powershell-invitations)


<h2> FILE DICTIONARY (Each file will have more documentation in the form of comments) </h2>
    <li> AdminAnnouncementEditor.js - Renders the announcement list and allows for addition/deletion (not hooked up to backend, refer to 5 in index) </li>
    <li> AdminWizardModal.js - Encapsulates the Editor and has multiple tabs to allow for easy addition of new features</li>
    <li> AlertsModal.js - This is hidden and not shown because it was never finished. Would be opened from the alert bell in the topnavbar and would have information about the users chat and lisence experation date.</li>
    <li> AllViewsModal.js - This is where the front-end filters are displayed.</li>
    <li> AnnouncementsDisplay.js - This is where the announcements on the home page are rendered.</li>
    <li> CalculateSelectedHelper.js - These are helper functions used by TextRange.js to determine what gets selected through the text "ranges"</li>
    <li> ColorThemes.js - contains the color codes for the different elements per color theme (outdated and needs to be updated which is why color theme selection is currently disabled in the settings)</li>
    <li>ComponentLoadMask.js - This is the loading spinner thats used to mask things while they are loading. It will take up all the available space of the parent and center the spinner vertically and horizontally.</li>
    <li>DateRange.js - This is not implemented due to lack of a sample for testing, a skeleton is in place but refer to TextRange.js and NumberRange.js for implementation.</li>
    <li>FilterSideBar.js - Calls loadVisualization upon mounting; Holds and passes down table data after being read. (split into 2 visual sections of top and bottom).</li>
    <li>FilterSideBarBottomView.js - Renders pinned and regular columns.</li>
    <li>FilterSideBarTopView.js - Renders the collapsible top section of the filter side bar which has the filter overview.</li>
    <li>FilterSummaryView.js - Creates the filter overview which allows you to track which filters are applied and remove filters column by column and quickly naviate to an applied filter.</li>
    <li>FilterTab.js - Holds the elastic list in one tab and the corresponding range to the data type in the second.</li>
    <li>FilterTable.js - Elastic table which is virtualized to save on performance.</li>
    <li>FloatingToggleButtons.js - The expandable bubble buttons located on the left side of the screen when inside of a visualization.</li>
    <li>ForceWindowLoad.js - Something Aditya was playing around with, left it so it was there if he came back.</li>
    <li>GeneralFunctions.js - Contains functions meant to be reused.</li>
    <li>GlyphLegend.js - Draggable glyph legend.</li>
    <li>HelpModal.js - This is not currently included since the help button in the navbar now directs you to JIRA.</li>
    <li>HomePage.js - Home page Master Component (Master components are components which get rendered by the react router, they will be parents to all contained components).</li>
    <li>index.js - Here is where all the redux reducers are defined, at the bottom of the page the react router is instanced.</li>
    <li>LoadingIcon.js - This is the spinning X that is used on initial landing of a page until resources can get loaded (wont happen when you move from login to homepage since this is a single page app but will happen on initial landing and upon refresh).</li>
    <li>LoadMaskHelper.js - Contains functions used to hide and show the LoadingIcon.</li>
    <li>Login.js - Login page Master Component.</li>
    <li>Logout.js - Envokes a logout.</li>
    <li>Maintenance.js - Maintenance page gets displayed if the node server goes down but the webviewer remains online.</li>
    <li>SavedViews.js - Saved views + shared views (shared views hidden since they were not implemented yet)</li>
    <li>NotFoundPage.js - Displayed when someone manually enters a url that doesnt exist</li>
    <li>NumberRange.js - Handles number ranges.</li>
    <li>PieChart.js - Defines the pie chart thats used in poll announcements.</li>
    <li>PinningViewsModal.js - Pop up that allows you to select views to be pinned.</li>
    <li>ViewsTab.js - Displays the recent views on the home page.</li>
    <li>Router.js - React router which handles what master component to display based on the url.</li>
    <li>SearchBox.js - Reusable seachbox which can be configured to apply on keypress or onblur.</li>
    <li>SelectedAndFilteredDisplay.js - Pop up for Selected Data and Filtered Data which displays a table and allows for download at csv.</li>
    <li>ServerCallHelper.js - Set of functions made to make servercalls easier.</li>
    <li>SettingsModal.js - Settings modal which is opened fromt he cog icon in the navbar.</li>
    <li>StatisticsModal.js - Modal which allows for selecting which stats to monitor and on which columns (it then opens the statistics draggable).</li>
    <li>TextRange.js - Handles ranges for text columns.</li>
    <li>TimeoutAlert.js - Modal that pops up when your session is expiring in 5 minutes.</li>
    <li>TopNavBar.js - Navbar.</li>
    <li>TutorialWindow.js - Tutorial walkthrough thats currently disabled for production since its not hooked to backend to see if the user already viewed the tutorial.</li>
    <li>UndoRedoChangeListener.js - Upon an appropriate change (so that a redo and an undo don't add to the history of changes) pushes or alters the history in the redux store.</li>
    <li>Userfeed.js - Twilio chat for inter-institution chat.</li>
    <li>ViewsManager.js - Views Funnel.</li>
    <li>VisualizationView - Visualization Master Component.</li>
    <li>VotingModal.js - Modal which opens when a poll announcement is clicked.</li>
    <li>XYZRemapModal.js - Modal which allows for changing of xyz fields.</li>

<h2>HOW TO ADD A NEW VIZ TO THE ALLOWED LIST</h2>
    <li>There is a better way to go about doing this than the way it is currently but but since it was temporary I chose a way that would edit the lists before they are sent to be mapped than add conditions while mapping.</li>
    <li>Near the top of the render method of ViewsManager.js, I parse through each fo the four lists and splice out any values that dont get matched to approved values.</li>
    <li>To add a new exception just add a check for it.</li>

<h2> HOW TO MAKE A NEW BUILD AND PUSH TO SERVER</h2>
    <li>Using cmd navigate to the path of the package.json of the react project you wish to build and run "npm run build"</li>
    <li>After the build is completed it will appear as a folder in the same location as package.json named "build", update the viz folder if necessary located build/Res/Viz</li>
    <li>Rename build to WebViewerApp</li>
    <li>In the EC2 server, navigate inside the WebViewer folder, and run ./webViewerServer.sh -k</li>
    <li>Run ./webViewerServer.sh -p to ensure that they were terminated properly</li>
    <li>In the EC2 server, Delete the folder named WebViewerApp-old, and rename WebViewerApp to WebViewerApp-old</li>
    <li>Upload WebViewerApp from your react project to the EC2 server</li>
    <li>After the upload is completed, run ./webViewerServer.sh -s to start the server again</li>
    <li>Run ./webViewerServer.sh -p to ensure that everything started properly</li>

<h2> HOW TO HOOK UP ANNOUNCEMENTS TO BACKEND</h2>
    Below are some sample server calls highlighting how this can be done

    Fetch Announcements.
    makeServerCall("manageAnnouncements",
        function (responseText) {
            var response = JSON.parse(responseText);
            debugger;
            //do-stuff
            context.props.dispatch( setTimer(new Date().getTime()) );
        },
        {
            post:true,
            data: { 
                operation:'fetch', //create/delete/fetch/pollResult
                config:{
                    //type: MultiPoll/SinglePoll/Release/Maintenance/Shout
                    //content: {whatever to put in the description}
                    //AnnouncementId: (if delete/pollResult request)
                    //options:["",""...] options for poll
                    //selectedOptionIds:[]
                } 
            }
        }
    );

    Poll Creation
    makeServerCall("manageAnnouncements",
        function (responseText) {
            var response = JSON.parse(responseText);
            debugger;
            //do-stuff
            context.props.dispatch( setTimer(new Date().getTime()) );
        },
        {
            post:true,
            data: { 
                operation:'create',
                config:{
                    type: 'SinglePoll',
                    content: "Question1",
                    options:["asdbc","asdfcv"]
                } 
            }
        }
    );

    Delete
    makeServerCall("manageAnnouncements",
        function (responseText) {
            var response = JSON.parse(responseText);
            debugger;
            //do-stuff
            context.props.dispatch( setTimer(new Date().getTime()) );
        },
        {
            post:true,
            data: { 
                operation:'delete',
                config:{
                    AnnouncementId:'13'
                } 
            }
        }
    );

    Poll result with save of user selection
    makeServerCall("manageAnnouncements",
        function (responseText) {
            var response = JSON.parse(responseText);
            debugger;
            //do-stuff
            context.props.dispatch( setTimer(new Date().getTime()) );
        },
        {
            post:true,
            data: { 
                operation:'pollResult',
                config:{
                    AnnouncementId:'12',
                    selectedOptionIds:[4]
                } 
            }
        }
    );

<h2>IMPLEMENTATION IDEAS FOR STANDALONE VIZS</h2>
    This entirely depends if you want people to be able to select front end filters.
    For both you would need to build a new backend to host it.
    If you want front-end filters/are allowing, then you need a landing page for that selection and you would need to use Aditya's multiple login for same user 'bucket' fix to store the subset databases in to prevent clashes.
    If you don't need front end filters or if there is only one viz then there is only one subset database and there will be no issues with clashes.

<h2>SERVEJS NODE MODULE CONFIGURATION</h2>
    When pushing a new build/instance of EC-2, make sure that the serve package is set to always serve the index.html file for a Single-Page Application (SPA) to account for server-side routing via BrowserRouter in React-Router-Dom.

    The easiest way to upload a new version of the package is to go to the root, then go to the path: /usr/lib/node_modules/serve/bin/serve.js 

    There, you can download and edit the file locally in your OS and reupload the file. Be sure to re-add execute permissions so that it can be run.

    On lines 390 to 397 of the serve package in node_modules, uncomment the code the if-statement so that the code always runs:

<h2>MICROSOFT AZURE ACTIVE DIRECTORY (AD) POWERSHELL INVITATIONS</h2>
    Follow this PowerShell guide: <a href="https://docs.microsoft.com/en-us/azure/active-directory/b2b/b2b-quickstart-invite-powershell">Guide here.</a>

    Sample command: 







    New-AzureADMSInvitation -InvitedUserDisplayName "bryanholster" -InvitedUserEmailAddress example@gmail.com -InviteRedirectURL https://glyphed.com -SendInvitationMessage $true


        

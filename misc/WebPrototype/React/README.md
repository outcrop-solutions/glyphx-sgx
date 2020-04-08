Table of Contents

File Dictionary <br>
1-to-1 chat<br>
How to add a new viz to allowed list<br>
How to make a new build and push to server<br>
How to hook up announcements to backend<br>
Implementation ideas for creating standalone vizs without a login<br>
Process to change the views manager funnel for viewer.synglyphx.com<br>
Important Note



1. FILE DICTIONARY (Each file will have more documentation in the form of comments)
    - AdminAnnouncementEditor.js - Renders the announcement list and allows for addition/deletion (not hooked up to backend, refer to 5 in index).
    - AdminWizardModal.js - Encapsulates the Editor and has multiple tabs to allow for easy addition of new features
    - AlertsModal.js - This is hidden and not shown because it was never finished. Would be opened from the alert bell in the topnavbar and would have information about the users chat and lisence experation date.
    - AllViewsModal.js - This is where the front-end filters are displayed.
    - AnnouncementsDisplay.js - This is where the announcements on the home page are rendered.
    - CalculateSelectedHelper.js - These are helper functions used by TextRange.js to determine what gets selected through the text "ranges"
    - ColorThemes.js - contains the color codes for the different elements per color theme (outdated and needs to be updated which is why color theme selection is currently disabled in the settings)
    - ComponentLoadMask.js - This is the loading spinner thats used to mask things while they are loading. It will take up all the available space of the parent and center the spinner vertically and horizontally.
    - DateRange.js - This is not implemented due to lack of a sample for testing, a skeleton is in place but refer to TextRange.js and NumberRange.js for implementation.
    - FilterSideBar.js - Calls loadVisualization upon mounting; Holds and passes down table data after being read. (split into 2 visual sections of top and bottom).
    - FilterSideBarBottomView.js - Renders pinned and regular columns.
    - FilterSideBarTopView.js - Renders the collapsible top section of the filter side bar which has the filter overview.
    - FilterSummaryView.js - Creates the filter overview which allows you to track which filters are applied and remove filters column by column and quickly naviate to an applied filter.
    - FilterTab.js - Holds the elastic list in one tab and the corresponding range to the data type in the second.
    - FilterTable.js - Elastic table which is virtualized to save on performance.
    - FloatingToggleButtons.js - The expandable bubble buttons located on the left side of the screen when inside of a visualization.
    - ForceWindowLoad.js - Something Aditya was playing around with, left it so it was there if he came back.
    - GeneralFunctions.js - Contains functions meant to be reused.
    - GlyphLegend.js - Draggable glyph legend.
    - HelpModal.js - This is not currently included since the help button in the navbar now directs you to JIRA.
    - HomePage.js - Home page Master Component (Master components are components which get rendered by the react router, they will be parents to all contained components).
    - index.js - Here is where all the redux reducers are defined, at the bottom of the page the react router is instanced.
    - LoadingIcon.js - This is the spinning X that is used on initial landing of a page until resources can get loaded (wont happen when you move from login to homepage since this is a single page app but will happen on initial landing and upon refresh).
    - LoadMaskHelper.js - Contains functions used to hide and show the LoadingIcon.
    - Login.js - Login page Master Component.
    - Logout.js - Envokes a logout.
    - Maintenance.js - Maintenance page gets displayed if the node server goes down but the webviewer remains online.
    - SavedViews.js - Saved views + shared views (shared views hidden since they were not implemented yet)
    - NotFoundPage.js - Displayed when someone manually enters a url that doesnt exist
    - NumberRange.js - Handles number ranges.
    - PieChart.js - Defines the pie chart thats used in poll announcements.
    - PinningViewsModal.js - Pop up that allows you to select views to be pinned.
    - ViewsTab.js - Displays the recent views on the home page.
    - Router.js - React router which handles what master component to display based on the url.
    - SearchBox.js - Reusable seachbox which can be configured to apply on keypress or onblur.
    - SelectedAndFilteredDisplay.js - Pop up for Selected Data and Filtered Data which displays a table and allows for download at csv.
    - ServerCallHelper.js - Set of functions made to make servercalls easier.
    - SettingsModal.js - Settings modal which is opened fromt he cog icon in the navbar.
    - StatisticsModal.js - Modal which allows for selecting which stats to monitor and on which columns (it then opens the statistics draggable).
    - TextRange.js - Handles ranges for text columns.
    - TimeoutAlert.js - Modal that pops up when your session is expiring in 5 minutes.
    - TopNavBar.js - Navbar.
    - TutorialWindow.js - Tutorial walkthrough thats currently disabled for production since its not hooked to backend to see if the user already viewed the tutorial.
    - UndoRedoChangeListener.js - Upon an appropriate change (so that a redo and an undo don't add to the history of changes) pushes or alters the history in the redux store.
    - Userfeed.js - Twilio chat for inter-institution chat.
    - ViewsManager.js - Views Funnel.
    - VisualizationView - Visualization Master Component.
    - VotingModal.js - Modal which opens when a poll announcement is clicked.
    - XYZRemapModal.js - Modal which allows for changing of xyz fields.

2. 1-TO-1 CHAT
    - login for twilio.com: user: marwane@synglyphx.com pass: SynGlyphX2013SGX!
    - First the front-end will need to adapt keeping in mind that there might be more than 50 users. So group chats at the top and users below based on most recent contact.
    - Refer to UserFeed.js to see how the twilio JS API is used.
    - To get the data for this we first need to determine the user list for the given institution. Using the channel id of the channel which contains all the institution's users (channels('CHXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX')), or channels unique name (getChannelByUniqueName(name)), call the method which returns a user list, (.members.each(members => console.log(members.sid)) will log all the members's sids of that channel).
    - Using this this populate the front-end with names and map their sids
    - open a new chat with the corresponding sid when their user name is clicked if one does not already exist from the front-end list

3. HOW TO ADD A NEW VIZ TO THE ALLOWED LIST
    - There is a better way to go about doing this than the way it is currently but but since it was temporary I chose a way that would edit the lists before they are sent to be mapped than add conditions while mapping.
    - Near the top of the render method of ViewsManager.js, I parse through each fo the four lists and splice out any values that dont get matched to approved values.
    - To add a new exception just add a check for it.

4. HOW TO MAKE A NEW BUILD AND PUSH TO SERVER
    - Using cmd navigate to the path of the package.json of the react project you wish to build and run "npm run build"
    - After the build is completed it will appear as a folder in the same location as package.json named "build", update the viz folder if necessary located build/Res/Viz
    - Rename build to WebViewerApp
    - In the EC2 server, navigate inside the WebViewer folder, and run ./webViewerServer.sh -k
    - Run ./webViewerServer.sh -p to ensure that they were terminated properly
    - In the EC2 server, Delete the folder named WebViewerApp-old, and rename WebViewerApp to WebViewerApp-old
    - Upload WebViewerApp from your react project to the EC2 server
    - After the upload is completed, run ./webViewerServer.sh -s to start the server again
    - Run ./webViewerServer.sh -p to ensure that everything started properly

5. HOW TO HOOK UP ANNOUNCEMENTS TO BACKEND
    - Below are some sample server calls highlighting how this can be done

    Fetch Announcements.
    ```makeServerCall("manageAnnouncements",
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
    ```

    Poll Creation
    ```
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
    ```

    Delete
    ```
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
    ```

    Poll result with save of user selection
    ```
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
    ```

6. IMPLEMENTATION IDEAS FOR STANDALONE VIZS
    This entirely depends if you want people to be able to select front end filters.
    For both you would need to build a new backend to host it.
    If you want front-end filters/are allowing, then you need a landing page for that selection and you would need to use Aditya's multiple login for same user 'bucket' fix to store the subset databases in to prevent clashes.
    If you don't need front end filters or if there is only one viz then there is only one subset database and there will be no issues with clashes.

7. HOW TO CHANGE THE FUNNEL FOR VIEWER.SYNGLYPHX.COM
    - In order for AllViewsModal.js (front-end filter selection Modal) to do its magic it requires two pieces of information for any given selection: Name + sdt Path
    - Sample: Global Admissions, C:\ProgramData\SynGlyphX\GlyphEd\Dev\Global Admissions\Global Admissions.sdt
    - The backend call made by AllViewsModal will take care of adjusting the desktop path to work for the ec2 server.
    - These two data elements are set as such within ViewsManager.js: this.setState({ selectionType: type[0], selectionTypeURL: type[1] })
    - Then to display the AllViewsModal all it takes is (the redux action for this is defined at the bottom of ViewsManager.js): this.props.dispatch(editModalDisplay(true))
    - So to change the funnel these two aspects are all that need to remain constant and the rest wil continue to work.

8. IMPORTANT NOTE
    - When pushing a new build/instance of EC-2, make sure that the serve package is set to always serve the index.html file for a Single-Page Application (SPA) to account for server-side routing via BrowserRouter in React-Router-Dom.

    On lines 390 to 397 of the serve package in node_modules, uncomment the code inside the if-statement but not the statement itself:

    ```
    if (args['--single']) {
		const {rewrites} = config;
		const existingRewrites = Array.isArray(rewrites) ? rewrites : [];

		// As the first rewrite rule, make `--single` work
		config.rewrites = [{
			source: '**',
			destination: '/index.html'
		}, ...existingRewrites];
	}
    ```

        

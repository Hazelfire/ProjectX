#define RESOURCE_FOLDER "res/"

	#define UI_FOLDER RESOURCE_FOLDER "ui/"

		#define TITLE_UI UI_FOLDER "title/"
			#define TITLE_BACKGROUND_IMAGE TITLE_UI "Title Background.png"
			#define TITLE_TEXT_IMAGE TITLE_UI "Title Text.png"

		#define GAME_UI_FOLDER UI_FOLDER "game/"

			#define INFO_BAR_IMAGE GAME_UI_FOLDER "InfoBar.png"
			#define INVENTORY_BUTTON GAME_UI_FOLDER "Inventory button.png"
			#define INTERACTIONS_SELECTOR GAME_UI_FOLDER "interactions selector.png"

		#define INVENTORY_FOLDER UI_FOLDER "inventory/"

			#define INVENTORY_BACKGROUND_IMAGE INVENTORY_FOLDER "Inventory Background.png"

			#define CRAFTING_IMAGE INVENTORY_FOLDER "Craft.png"

		#define PLAYER_SELECTION_FOLDER UI_FOLDER "player_selection/"

			#define PLAYER_SELECTION_BACKGROUND PLAYER_SELECTION_FOLDER "Old Paper Background.png"
			#define HUE_BAR_IMAGE PLAYER_SELECTION_FOLDER "hue.png"
			#define LEFT_ARROW_IMAGE PLAYER_SELECTION_FOLDER "Left Arrow.png"
			#define RIGHT_ARROW_IMAGE PLAYER_SELECTION_FOLDER "Right Arrow.png"
			#define SATURATION_BAR_IMAGE PLAYER_SELECTION_FOLDER "saturation.png"
			#define VALUE_BAR_IMAGE PLAYER_SELECTION_FOLDER "value.png"
			#define UNDERLAY_BAR_IMAGE PLAYER_SELECTION_FOLDER "underlay.png"
			#define SLIDERBALL_IMAGE PLAYER_SELECTION_FOLDER "sliderball.png"
			#define START_IMAGE PLAYER_SELECTION_FOLDER "Start.png"
			#define DETAILS_PREVIEWER_BACKGROUND PLAYER_SELECTION_FOLDER "Details Background.png"

		#define MULTIPLAYER_LOBBY_FOLDER UI_FOLDER "multiplayer_lobby/"

			#define LOBBY_BACKGROUND MULTIPLAYER_LOBBY_FOLDER "background.png"

		#define DIALOGUE_FOLDER UI_FOLDER "Dialogue/"

			#define DIALOGUE_BACKGROUND DIALOGUE_FOLDER "Dialogue Background.png"

		#define MAP_SELECTION_FOLDER UI_FOLDER "map_selection/"

			#define MAP_SELECTION_BACKGROUND MAP_SELECTION_FOLDER "map_selection_background.png"

		#define OPTIONS_UI_FOLDER UI_FOLDER "options_menu/"

			#define OPTIONS_BACKGROUND OPTIONS_UI_FOLDER "background.png"

		#define ERROR_SCREEN_FOLDER UI_FOLDER "error_screen/"

			#define ERROR_SCREEN_BACKGROUND ERROR_SCREEN_FOLDER "background.png"

		#define SPLASH_FOLDER UI_FOLDER "splash/"

			#define COCOS2DX_SPLASH SPLASH_FOLDER "cocos2dx.png"
			#define PROJECTX_SPLASH SPLASH_FOLDER "ProjectX_Splash.png"

		#define DEV_OPTIONS_FOLDER UI_FOLDER "dev_options/"

			#define DEV_OPTIONS_BACKGROUND DEV_OPTIONS_FOLDER "background.jpg"

		#define EXIT_MENU_FOLDER UI_FOLDER "exit_menu/"

			#define EXIT_MENU_BACKGROUND EXIT_MENU_FOLDER "ExitMenuBackground.png"

	#define FONTS_FOLDER RESOURCE_FOLDER "fonts/"

	#define MUSIC_FOLDER RESOURCE_FOLDER "music/"

	#define PLAYERS_FOLDER RESOURCE_FOLDER "players/"

	#define SPRITES_FOLDER RESOURCE_FOLDER "sprites/"	

#define SCRIPTS_FOLDER "script/"

		#define LUA_FOLDER SCRIPTS_FOLDER "lua/"

			#define LUA_INDEX_FILE LUA_FOLDER "lua_index.yml"

		#define XML_FOLDER SCRIPTS_FOLDER "xml/"

			#define XML_INDEX_FILE XML_FOLDER "xml_index.yml"

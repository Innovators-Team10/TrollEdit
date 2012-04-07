-- configuration file
width = 800
height = 600
style = "QMainWindow {"
                        .."color: black;"
                        .."background-color: #C2C2C2;"
                        .."border-color: silver;"
                        .."font-size: 10px;"
                     .."}"

                    .."QTabWidget::tab-bar {"
                        .."left: 2px; /* odsadenie od plus*/"
                    .."}"

                    .."QTabBar::tab {"
                        .."background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						.."stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
						.."stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                        .."border: 1px solid silver; /* vrchne oramovanie tabu*/"

                        .."border-top-left-radius: 7px;"
                        .."border-top-right-radius: 7px;"
                        .."min-width: 18ex;"
                        .."padding: 5px; /*vyska tabu*/"
                    .."}"

                    .."QTabBar::tab:selected, QTabBar::tab:hover {"
                        .."background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						.."stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
						.."stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                    .."}"

                    .."QTabBar::tab:selected {"
                        .."border-color: #9B9B9B;"
						.."border-bottom-color: red;"
                    .."}"

					.."QTabBar::tab:!selected {"
                        .."margin-top: 2px;"
					.."}"

                    .."QMenu {"
						.."background-color: #D6DADE; /*vnutro menu*/"
                        .."color: white;"
                    .."}"

                    .."QMenu::item {"
                        .."background-color: transparent;"
                        .."color: black;"
                    .."}"

                    .."QMenu::item:selected {"
                        .."background-color: #A6A8AB;"
                    .."}"

                    .."QMenu::item:disabled {"
                        .."color:gray;"
                    .."}"


                    .."QMenuBar {"
                        .."background-color: #787A7C;"
                        .."color: white;"
                    .."}"

                    .."QMenuBar::item {"
                        .."spacing: 13px;"
                        .."padding: 2px 12px;"
                        .."background: transparent;"
                        .."border-radius: 2px;"
                    .."}"

                    .."QMenuBar::item:selected {"
                        .."background: #a8a8a8;"
                    .."}"

                    .."QMenuBar::item:disabled {"
                        .."background: #000000;"
                    .."}"

                    .."QMenuBar::item:pressed {"
                        .."background: #888888;"
                    .."}"


                    .."QToolButton {"
                        .."padding: 2px;"
                        .."background-color: transparent;"
                        .."min-width: 35px;"
                    .."}"

                    .."QToolButton:hover {"
                        .."background: qradialgradient(cx: 0.3, cy: -0.4,"
                        .."fx: 0.3, fy: -0.4,"
                        .."radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                    .."}"

                    .."QToolButton:pressed {"
                        .."background: qradialgradient(cx: 0.4, cy: -0.1,"
                        .."fx: 0.4, fy: -0.1,"
                        .."radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                    .."}"

                    .."QToolButton:disabled {"
                        .."background: transparent"
                        .."fx: 0.4, fy: -0.1,"
                        .."radius: 1.35, stop: 0 #777, stop: 1 #333);"
                    .."}"

                    .."QStatusBar{ "
                        .."color: black;"
                    .."}"
                .."QMessageBox {"
                .."background-color:silver;"
                .."}"

setstyle(style)

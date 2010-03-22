# -------------------------------------------------
# Project created by QtCreator 2009-10-21T20:18:13
# -------------------------------------------------
TARGET = TrollEditor
TEMPLATE = app
SOURCES += widget/main.cpp \
    widget/main_window.cpp \
    widget/document_tabs.cpp \
    widget/document_scene.cpp \
#
    gui/block.cpp \
    gui/hide_block_button.cpp \
    gui/text_item.cpp \
#
    analysis/language_manager.cpp \
    analysis/analyzer.cpp \
    analysis/tree_element.cpp \
    analysis/paired_tree_element.cpp
#
HEADERS += widget/main_window.h \
    widget/document_tabs.h \
    widget/document_scene.h \
#
    gui/block.h \
    gui/hide_block_button.h \
    gui/text_item.h \
#
    analysis/language_manager.h \
    analysis/analyzer.h \
    analysis/tree_element.h \
    analysis/paired_tree_element.h
#
RESOURCES += resources.qrc
INCLUDEPATH += "lua\include"
LIBS += "lua\lua5.1.dll"

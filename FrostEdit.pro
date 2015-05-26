#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T01:18:09
#
#-------------------------------------------------

QT += core gui xml network concurrent webkitwidgets

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FrostEdit
TEMPLATE = app


SOURCES += main.cpp\
    TextEditor/document.cpp \
    TextEditor/textedit.cpp \
    tabwidget.cpp \
    frostedit.cpp \
    tabwidgetframe.cpp \
    console.cpp \
    frostdialog.cpp \
    combobox.cpp \
    TextEditor/qate/context.cpp \
    TextEditor/qate/defaultcolors.cpp \
    TextEditor/qate/definitiondownloader.cpp \
    TextEditor/qate/dynamicrule.cpp \
    TextEditor/qate/highlightdefinition.cpp \
    TextEditor/qate/highlightdefinitionhandler.cpp \
    TextEditor/qate/highlightdefinitionmanager.cpp \
    TextEditor/qate/highlightdefinitionmetadata.cpp \
    TextEditor/qate/highlighter.cpp \
    TextEditor/qate/highlightersettings.cpp \
    TextEditor/qate/includerulesinstruction.cpp \
    TextEditor/qate/itemdata.cpp \
    TextEditor/qate/keywordlist.cpp \
    TextEditor/qate/mimedatabase.cpp \
    TextEditor/qate/progressdata.cpp \
    TextEditor/qate/rule.cpp \
    TextEditor/qate/specificrules.cpp \
    issuelist.cpp \
    settingsmenu.cpp \
    TextEditor/qate/tabsettings.cpp \
    TextEditor/fatehighlighter.cpp


HEADERS  += \
    TextEditor/document.hpp \
    TextEditor/textedit.hpp \
    tabwidget.hpp \
    frostedit.hpp \
    tabwidgetframe.hpp \
    console.hpp \
    frostdialog.hpp \
    combobox.hpp \
    TextEditor/qate/basetextdocumentlayout.h \
    TextEditor/qate/context.h \
    TextEditor/qate/defaultcolors.h \
    TextEditor/qate/definitiondownloader.h \
    TextEditor/qate/dynamicrule.h \
    TextEditor/qate/highlightdefinition.h \
    TextEditor/qate/highlightdefinitionhandler.h \
    TextEditor/qate/highlightdefinitionmanager.h \
    TextEditor/qate/highlightdefinitionmetadata.h \
    TextEditor/qate/highlighter.h \
    TextEditor/qate/highlighterexception.h \
    TextEditor/qate/highlightersettings.h \
    TextEditor/qate/includerulesinstruction.h \
    TextEditor/qate/itemdata.h \
    TextEditor/qate/keywordlist.h \
    TextEditor/qate/mimedatabase.h \
    TextEditor/qate/progressdata.h \
    TextEditor/qate/qateblockdata.h \
    TextEditor/qate/reuse.h \
    TextEditor/qate/rule.h \
    TextEditor/qate/specificrules.h \
    TextEditor/qate/syntaxhighlighter.h \
    TextEditor/qate/tabsettings.h \
    TextEditor/qate/utils/qtcassert.h \
    issuelist.hpp \
    settingsmenu.hpp \
    fatedata.hpp \
    TextEditor/fatedata.hpp \
    TextEditor/fatehighlighter.hpp


FORMS    += \
    tabwidget.ui \
    frostedit.ui \
    tabwidgetframe.ui \
    frostdialog.ui \
    settingsmenu.ui

RESOURCES += \
    icons.qrc \
    icons.qrc

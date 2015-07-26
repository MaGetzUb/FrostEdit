#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T01:18:09
#
#-------------------------------------------------

QT += core gui xml network concurrent webkitwidgets

CONFIG += c++14
TEMPLATE = app
TARGET = FrostEdit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


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
    TextEditor/fatehighlighter.cpp \
    settings.cpp \
    TextEditor/codemodel.cpp \
    TextEditor/frostcodemodel.cpp \
    TextEditor/frosttoken.cpp \
    quitdialog.cpp \
    TextEditor/findreplacedialog.cpp \
    TextEditor/frostcodemodelcontext.cpp \
    appearancetab.cpp \
    generalsettingstab.cpp \
    colorscheme.cpp \
    TextEditor/codepoint.cpp \
    documentitem.cpp \
    filelistwidget.cpp \
    combotabwidget.cpp \
    filesystemmodel.cpp

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
    TextEditor/fatehighlighter.hpp \
    documentitem.hpp \
    settings.hpp \
    TextEditor/codemodel.hpp \
    TextEditor/codepoint.hpp \
    TextEditor/frostcodemodel.hpp \
    TextEditor/symbol.hpp \
    TextEditor/frosttoken.hpp \
    quitdialog.hpp \
    TextEditor/findreplacedialog.hpp \
    syntaxstyle.hpp \
    TextEditor/documentmap.hpp \
    TextEditor/frostcodemodelcontext.hpp \
    TextEditor/tokenbase.hpp \
    TextEditor/linenumberarea.hpp \
    appearancetab.hpp \
    generalsettingstab.hpp \
    colorscheme.hpp \
    filelistwidget.hpp \
    combotabwidget.hpp \
    filesystemmodel.hpp

FORMS    += \
    tabwidget.ui \
    frostedit.ui \
    tabwidgetframe.ui \
    frostdialog.ui \
    settingsmenu.ui \
    quitdialog.ui \
    TextEditor/findreplacedialog.ui \
    appearancetab.ui \
    generalsettingstab.ui \
    combotabwidget.ui

RESOURCES += \
    resources.qrc

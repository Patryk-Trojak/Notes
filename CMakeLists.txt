cmake_minimum_required(VERSION 3.5)

project(Notes VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(FORMS_DIR ${CMAKE_SOURCE_DIR}/forms)
set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${FORMS_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

set(PROJECT_SOURCES
        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/MainWindow.cpp ${INCLUDE_DIR}/MainWindow.h
        ${INCLUDE_DIR}/NoteData.h ${SOURCE_DIR}/NoteData.cpp
        ${INCLUDE_DIR}/NoteButton.h ${SOURCE_DIR}/NoteButton.cpp ${FORMS_DIR}/NoteButton.ui
        ${INCLUDE_DIR}/NoteEditingTab.h ${SOURCE_DIR}/NoteEditingTab.cpp
        ${INCLUDE_DIR}/NotesDisplayingTab.h ${SOURCE_DIR}/NotesDisplayingTab.cpp ${FORMS_DIR}/NotesDisplayingTab.ui
        ${INCLUDE_DIR}/PersistenceManager.h ${SOURCE_DIR}/PersistenceManager.cpp
        ${INCLUDE_DIR}/Utils.h ${SOURCE_DIR}/Utils.cpp
        ${INCLUDE_DIR}/AboutWindow.h ${SOURCE_DIR}/AboutWindow.cpp ${FORMS_DIR}/AboutWindow.ui
        ${RESOURCES_DIR}/images.qrc
        ${INCLUDE_DIR}/SearchBar.h ${SOURCE_DIR}/SearchBar.cpp
        ${INCLUDE_DIR}/FolderData.h ${SOURCE_DIR}/FolderData.cpp
        ${INCLUDE_DIR}/FolderTreeModel.h ${SOURCE_DIR}/FolderTreeModel.cpp
        ${INCLUDE_DIR}/FolderTreeItem.h ${SOURCE_DIR}/FolderTreeItem.cpp
        ${INCLUDE_DIR}/FolderTreeView.h ${SOURCE_DIR}/FolderTreeView.cpp
        ${INCLUDE_DIR}/NoteListModel.h ${SOURCE_DIR}/NoteListModel.cpp
        ${INCLUDE_DIR}/NoteListDelegate.h ${SOURCE_DIR}/NoteListDelegate.cpp
        ${INCLUDE_DIR}/NoteListView.h ${SOURCE_DIR}/NoteListView.cpp
        ${INCLUDE_DIR}/NoteSortFilterProxyModel.h ${SOURCE_DIR}/NoteSortFilterProxyModel.cpp
        ${INCLUDE_DIR}/ElidedLabel.h ${SOURCE_DIR}/ElidedLabel.cpp
        ${INCLUDE_DIR}/ColorButton.h ${SOURCE_DIR}/ColorButton.cpp
        ${INCLUDE_DIR}/ColorPicker.h ${SOURCE_DIR}/ColorPicker.cpp
        ${INCLUDE_DIR}/NoteEditor.h ${SOURCE_DIR}/NoteEditor.cpp ${FORMS_DIR}/NoteEditor.ui
        ${INCLUDE_DIR}/FolderTreeDelegate.h ${SOURCE_DIR}/FolderTreeDelegate.cpp
        ${INCLUDE_DIR}/NoteSortOptionsWidget.h ${SOURCE_DIR}/NoteSortOptionsWidget.cpp ${FORMS_DIR}/NoteSortOptionsWidget.ui
        ${INCLUDE_DIR}/NoteListViewSelectionMenu.h ${SOURCE_DIR}/NoteListViewSelectionMenu.cpp
        ${INCLUDE_DIR}/NoteListModelRole.h ${INCLUDE_DIR}/FolderTreeModelRole.h
        ${INCLUDE_DIR}/NoteMimeData.h ${SOURCE_DIR}/NoteMimeData.cpp
        ${INCLUDE_DIR}/FolderMimeData.h ${SOURCE_DIR}/FolderMimeData.cpp
        ${INCLUDE_DIR}/SpecialFolderId.h
        ${INCLUDE_DIR}/NoteContentEdit.h ${SOURCE_DIR}/NoteContentEdit.cpp
        ${INCLUDE_DIR}/ImageDownloader.h ${SOURCE_DIR}/ImageDownloader.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Notes
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Notes APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Notes SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Notes
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Notes PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(Notes PRIVATE ${INCLUDE_DIR})
set_target_properties(Notes PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Notes
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Notes)
endif()

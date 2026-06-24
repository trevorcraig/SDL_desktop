#include <stdio.h>
#include <string.h>

#include <sqlite3.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "panel.h"
#include "button.h"
#include "textbox.h"
#include "label.h"

#define MAX_CONTACTS 256

//--------------------------------------
// Contact
//--------------------------------------

typedef struct
{
    char name[64];
    char phone[64];

} Contact;

static Contact contacts[
    MAX_CONTACTS
];

static int contactCount =
    0;

//--------------------------------------
// UI
//--------------------------------------

static Panel listPanel;

static TextBox nameBox;
static TextBox phoneBox;

static Label rows[
    MAX_CONTACTS
];

static TTF_Font* font;

static sqlite3* db;

//--------------------------------------
// Database
//--------------------------------------

static void CreateDatabase()
{
    sqlite3_open(
        "contacts.db",
        &db
    );

    const char* sql =
        "CREATE TABLE IF NOT EXISTS contacts("
        "id INTEGER PRIMARY KEY,"
        "name TEXT,"
        "phone TEXT"
        ");";

    sqlite3_exec(
        db,
        sql,
        NULL,
        NULL,
        NULL
    );
}

static void InsertContact(
    const char* name,
    const char* phone
)
{
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(
        db,
        "INSERT INTO contacts(name,phone)"
        " VALUES(?,?)",
        -1,
        &stmt,
        NULL
    );

    sqlite3_bind_text(
        stmt,
        1,
        name,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        stmt,
        2,
        phone,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_step(
        stmt
    );

    sqlite3_finalize(
        stmt
    );
}

static int LoadCallback(
    void* unused,
    int argc,
    char** argv,
    char** cols
)
{
    (void)unused;
    (void)argc;
    (void)cols;

    if (
        contactCount >=
        MAX_CONTACTS
    )
    {
        return 0;
    }

    strcpy(
        contacts[
            contactCount
        ].name,
        argv[0]
    );

    strcpy(
        contacts[
            contactCount
        ].phone,
        argv[1]
    );

    contactCount++;

    return 0;
}

static void LoadContacts()
{
    contactCount =
        0;

    sqlite3_exec(
        db,
        "SELECT name,phone "
        "FROM contacts",
        LoadCallback,
        NULL,
        NULL
    );
}

//--------------------------------------
// UI
//--------------------------------------

static void RefreshList()
{
    listPanel.count =
        0;

    static char text[
        MAX_CONTACTS
    ][128];

    for (
        int i=0;
        i<contactCount;
        i++
    )
    {
        SDL_snprintf(
            text[i],
            sizeof(text[i]),
            "%s | %s",
            contacts[i].name,
            contacts[i].phone
        );

        Label_Init(
            &rows[i],
            0,
            0,
            text[i],
            font
        );

        Panel_Add(
            &listPanel,
            (Widget*)
            &rows[i]
        );
    }
}

static void AddContact()
{
    if (
        !nameBox.text[0]
        ||
        !phoneBox.text[0]
    )
    {
        return;
    }

    InsertContact(
        nameBox.text,
        phoneBox.text
    );

    LoadContacts();

    RefreshList();

    nameBox.text[0] =
        '\0';

    phoneBox.text[0] =
        '\0';

    nameBox.length =
        0;

    phoneBox.length =
        0;
}

//--------------------------------------
// Main
//--------------------------------------

int main()
{
    SDL_Init(
        SDL_INIT_VIDEO
    );

    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow(
            "SQLite Contacts",
            700,
            700,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    SDL_StartTextInput(
        window
    );

    //----------------------------------

    char fontPath[512];

    SDL_snprintf(
        fontPath,
        sizeof(fontPath),
        "%sPTC55F.ttf",
        SDL_GetBasePath()
    );

    font =
        TTF_OpenFont(
            fontPath,
            24
        );

    //----------------------------------

    CreateDatabase();

    LoadContacts();

    //----------------------------------

    Panel root;

    Panel_Init(
        &root,
        0,
        0,
        700,
        700
    );

    Panel_SetLayout(
        &root,
        PANEL_VERTICAL
    );

    Label title;

    Label_Init(
        &title,
        0,
        0,
        "SQLite Contacts",
        font
    );

    Panel_Add(
        &root,
        (Widget*)&title
    );

    TextBox_Init(
        &nameBox,
        0,
        0,
        600,
        60,
        font
    );

    Panel_Add(
        &root,
        (Widget*)&nameBox
    );

    TextBox_Init(
        &phoneBox,
        0,
        0,
        600,
        60,
        font
    );

    Panel_Add(
        &root,
        (Widget*)&phoneBox
    );

    Button add;

    Button_Init(
        &add,
        0,
        0,
        300,
        60,
        "Save Contact",
        font,
        AddContact
    );

    Panel_Add(
        &root,
        (Widget*)&add
    );

    Panel_Init(
        &listPanel,
        0,
        0,
        650,
        350
    );

    Panel_EnableScroll(
        &listPanel
    );

    Panel_Add(
        &root,
        (Widget*)&listPanel
    );

    RefreshList();

    //----------------------------------

    bool running =
        true;

    while (
        running
    )
    {
        SDL_Event e;

        while (
            SDL_PollEvent(
                &e
            )
        )
        {
            if (
                e.type ==
                SDL_EVENT_QUIT
            )
            {
                running =
                    false;
            }

            Widget_Handle(
                (Widget*)
                &root,
                &e
            );
        }

        SDL_SetRenderDrawColor(
            renderer,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(
            renderer
        );

        Widget_Render(
            (Widget*)
            &root,
            renderer
        );

        SDL_RenderPresent(
            renderer
        );
    }

    //----------------------------------

    sqlite3_close(
        db
    );

    TTF_CloseFont(
        font
    );

    SDL_StopTextInput(
        window
    );

    SDL_DestroyRenderer(
        renderer
    );

    SDL_DestroyWindow(
        window
    );

    TTF_Quit();

    SDL_Quit();

    return 0;
}
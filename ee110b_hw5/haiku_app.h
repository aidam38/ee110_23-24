/****************************************************************************/
/*                                                                          */
/*                              haiku_app.h                                 */
/*                       Haiku Application Header                           */
/*                                                                          */
/****************************************************************************/

/* Haiku Application Header. Functions declared are:
        App_init() - initialize the application
        App_run() - main application task
        App_processEvent() - process events from the event queue
        haiku1, haiku2, haiku3, haiku4 - haiku data to display

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

#ifndef  __HAIKU_APP_H__
    #define  __HAIKU_APP_H__

/* local include files */
#include  "haiku_app_intf.h"

#define APP_TASK_PRIORITY          	2
#define APP_TASK_STACK_SIZE 		1024

/* structs */

/* event struct */
typedef struct event {
    Queue_Elem elem;
    uint32_t data;
} event_t;

/* line part struct */
typedef struct part {
    uint32_t row;   // row where to display
    uint32_t col;   // column where to display
    char *text;     // text to display
} part_t;

/* line sturct */
typedef struct line {
    part_t parts[4]; // each line is expected to have 4 parts
} line_t;

/* haiku struct */
typedef struct haiku {
    line_t lines[4]; // each haiku is expected to have 4 lines (3 verses and 1 source)
} haiku_t;


/* internal function declarations */
void App_init(void);
static void App_run(UArg a0, UArg a1);
void App_processEvent(event_t* evt);


/* haiku data */
const haiku_t haiku1 = {{
                        {{ { 0, 0, "Tiny" }, { 1, 4, "chips" }, { 2, 7, "hum" }, { 3, 10, "softly" } }},
                        {{ { 0, 0, "Logic" }, { 1, 1, "weaves through" }, { 2, 5, "circuits" }, { 3, 11, "tight" } }},
                        {{ { 0, 0, "Worlds" }, { 1, 4, "in code" }, { 2, 7, "take" }, { 3, 10, "flight" } }},
                        {{ { 1, 4, "3/6/2024" }, { 2, 1, "OpenAI ChatGPT" }, { 0, 0, "" }, { 0, 0, "" } }},
}};

const haiku_t haiku2 = {{
                        {{ { 0, 0, "Silent" }, { 1, 4, "lines" }, { 2, 8, "of" }, { 3, 10, "code" } }},
                        {{ { 0, 0, "Tiny" }, { 1, 1, "worlds" }, { 2, 5, "within" }, { 3, 8, "circuits" } }},
                        {{ { 0, 0, "Whispers" }, { 1, 4, "of" }, { 2, 7, "machines" }, { 3, 10, "" } }},
                        {{ { 1, 4, "3/6/2024" }, { 2, 1, "Microsoft Bing" }, { 0, 0, "" }, { 0, 0, "" } }},
}};

const haiku_t haiku3 = {{
                        {{ { 0, 0, "Tiny" }, { 1, 4, "heart" }, { 2, 8, "of" }, { 3, 10, "code" } }},
                        {{ { 0, 0, "Whispers" }, { 1, 1, "life" }, { 2, 2, "to" }, { 3, 3, "hidden things" } }},
                        {{ { 0, 0, "Makes" }, { 1, 4, "the world" }, { 2, 7, "hum" }, { 3, 10, "soft" } }},
                        {{ { 1, 4, "3/6/2024" }, { 2, 2, "Google Gemini" }, { 0, 0, "" }, { 0, 0, "" } }},
}};

const haiku_t haiku4 = {{
                        {{ { 0, 0, "Tiny" }, { 1, 4, "circuits" }, { 2, 13, "hum" }, { 3, 10, "" } }},
                        {{ { 0, 0, "Invisible" }, { 2, 9, "yet" }, { 3, 11, "vital" }, { 3, 11, "" } }},
                        {{ { 0, 0, "World" }, { 1, 4, "runs" }, { 2, 6, "on their" }, { 3, 12, "code" } }},
                        {{ { 1, 4, "3/6/2024" }, { 2, 0, "Anthropic Claude" }, { 0, 0, "" }, { 0, 0, "" } }},
}};

#endif

/* FrogGBA UI texts for the tabbed menu and the home screen, in English and French.
 *
 * Accented letters are stored as control codes 1..31 and drawn by mh_print.c from
 * generated glyphs (see accent_font there). Use the macros below inside strings and
 * keep a letter or a space between two macros.
 */

#ifndef UI_TEXT_H
#define UI_TEXT_H

#define LANG_JAPANESE 0
#define LANG_ENGLISH  1
#define LANG_FRENCH   2
#define LANG_COUNT    3

/* e acute, e grave, e circumflex, a grave, a circumflex, c cedilla, u grave */
#define A_EACUTE   "\001"
#define A_EGRAVE   "\002"
#define A_ECIRC    "\003"
#define A_AGRAVE   "\004"
#define A_ACIRC    "\005"
#define A_CCEDIL   "\006"
#define A_UGRAVE   "\007"
/* u circumflex, o circumflex, i circumflex, i diaeresis */
#define A_UCIRC    "\013"
#define A_OCIRC    "\014"
#define A_ICIRC    "\016"
#define A_IDIAER   "\017"
/* capital E acute, capital E grave, capital A grave, e diaeresis */
#define A_CAP_EACUTE "\020"
#define A_CAP_EGRAVE "\021"
#define A_CAP_AGRAVE "\022"
#define A_EDIAER   "\023"

typedef enum
{
  UI_TAB_GAME,
  UI_TAB_SCREEN,
  UI_TAB_PERF,
  UI_TAB_PAD,
  UI_TAB_SYSTEM,

  UI_GAME_RETURN,
  UI_GAME_SAVE,
  UI_GAME_LOAD,
  UI_GAME_DETAILS,
  UI_GAME_CAPTURE,
  UI_GAME_CHEATS,
  UI_GAME_RESET,
  UI_GAME_LOADGAME,

  UI_SCR_SCALE,
  UI_SCR_ZOOM,
  UI_SCR_ASPECT,
  UI_SCR_FILTER,
  UI_SCR_COLOR,
  UI_SCR_FPS,
  UI_SCR_OVERLAY,

  UI_PERF_CPU,
  UI_PERF_FSKIP,
  UI_PERF_FSKIPN,
  UI_PERF_VOL,
  UI_PERF_COMPAT,
  UI_PERF_STACK,

  UI_PAD_BUTTONS,
  UI_PAD_ANALOG,
  UI_PAD_MAPPING,

  UI_SYS_LANG,
  UI_SYS_BIOS,
  UI_SYS_BACKUP,
  UI_SYS_SAVING,
  UI_SYS_DEFAULTS,
  UI_SYS_QUIT,

  UI_D_RETURN,
  UI_D_SAVE,
  UI_D_LOAD,
  UI_D_DETAILS,
  UI_D_CAPTURE,
  UI_D_CHEATS,
  UI_D_RESET,
  UI_D_LOADGAME,
  UI_D_SCALE,
  UI_D_ZOOM,
  UI_D_ASPECT,
  UI_D_FILTER,
  UI_D_COLOR,
  UI_D_FPS,
  UI_D_OVERLAY,
  UI_D_CPU,
  UI_D_FSKIP,
  UI_D_FSKIPN,
  UI_D_VOL,
  UI_D_COMPAT,
  UI_D_STACK,
  UI_D_BUTTONS,
  UI_D_ANALOG,
  UI_D_MAPPING,
  UI_D_LANG,
  UI_D_BIOS,
  UI_D_BACKUP,
  UI_D_SAVING,
  UI_D_DEFAULTS,
  UI_D_QUIT,

  UI_HINT_SELECT,
  UI_HINT_BACK,
  UI_HINT_TABS,
  UI_HINT_CHANGE,
  UI_FILTER_SHARP,
  UI_LANG_FRENCH,

  UI_END
} UI_TEXT_ID;

extern const char *ui_text_table[LANG_COUNT][UI_END];

/* Japanese uses the English texts for everything that is new. */
#define UI_LANG_INDEX() ((option_language == LANG_FRENCH) ? LANG_FRENCH : LANG_ENGLISH)
#define UI(id)          (ui_text_table[UI_LANG_INDEX()][id])

#endif

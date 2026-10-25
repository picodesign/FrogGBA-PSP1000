/*****************************************************************************
 * 全角文字列表示ライブラリ     mediumgauge
 *****************************************************************************/

#include "common.h"

#include "shinonomefont.c"

/*****************************************************************************
* prots
*****************************************************************************/
static void draw_char(s32 chr, u16 x, u16 y, u16 col, s16 bg_col, u16 *base_vram, u16 bufferwidth);

/*****************************************************************************
* Accented letters for the French UI. Control codes 1..31 (except '\n') are
* drawn from glyphs generated at first use from the base letter plus a mark.
*****************************************************************************/
#define ACCENT_IDX_BASE (0x1000)

enum { ACC_ACUTE, ACC_GRAVE, ACC_CIRC, ACC_DIAER, ACC_CEDIL, ACC_CAP_ACUTE, ACC_CAP_GRAVE };

static const struct { u8 code; u8 base; u8 mark; } accent_table[] =
{
  {  1, 'e', ACC_ACUTE },     /* e acute */
  {  2, 'e', ACC_GRAVE },     /* e grave */
  {  3, 'e', ACC_CIRC },      /* e circumflex */
  {  4, 'a', ACC_GRAVE },     /* a grave */
  {  5, 'a', ACC_CIRC },      /* a circumflex */
  {  6, 'c', ACC_CEDIL },     /* c cedilla */
  {  7, 'u', ACC_GRAVE },     /* u grave */
  { 11, 'u', ACC_CIRC },      /* u circumflex */
  { 12, 'o', ACC_CIRC },      /* o circumflex */
  { 14, 'i', ACC_CIRC },      /* i circumflex */
  { 15, 'i', ACC_DIAER },     /* i diaeresis */
  { 16, 'E', ACC_CAP_ACUTE }, /* E acute */
  { 17, 'E', ACC_CAP_GRAVE }, /* E grave */
  { 18, 'A', ACC_CAP_GRAVE }, /* A grave */
  { 19, 'e', ACC_DIAER },     /* e diaeresis */
};

static u8 accent_font[32 * 9];
static u8 accent_font_ready = 0;

/* A glyph is 12 rows of 6 bits packed MSB first; bit 5 of a row is the left pixel. */
static void accent_set(u8 *rows, u32 row, const char *pattern)
{
  u32 x;
  for (x = 0; x < 6; x++)
    if (pattern[x] == '#')
      rows[row] |= (u8)(0x20 >> x);
}

static void accent_init(void)
{
  u32 n, r, i;

  memset(accent_font, 0, sizeof(accent_font));

  for (n = 0; n < sizeof(accent_table) / sizeof(accent_table[0]); n++)
  {
    u8 rows[12];
    const u8 *src = &shinonomefont[(accent_table[n].base - 0x20) * 9];
    u32 bit = 0;

    for (r = 0; r < 12; r++)
    {
      rows[r] = 0;
      for (i = 0; i < 6; i++, bit++)
        if (src[bit >> 3] & (0x80 >> (bit & 7)))
          rows[r] |= (u8)(0x20 >> i);
    }

    switch (accent_table[n].mark)
    {
      case ACC_ACUTE:      accent_set(rows, 1, "...#.."); accent_set(rows, 2, "..#..."); break;
      case ACC_GRAVE:      accent_set(rows, 1, ".#...."); accent_set(rows, 2, "..#..."); break;
      case ACC_CIRC:
        if (accent_table[n].base == 'i') rows[1] = 0;
        accent_set(rows, 1, "..#..."); accent_set(rows, 2, ".#.#..");
        break;
      case ACC_DIAER:
        if (accent_table[n].base == 'i') { rows[1] = 0; accent_set(rows, 1, ".#.#.."); }
        else accent_set(rows, 2, ".#.#..");
        break;
      case ACC_CEDIL:      accent_set(rows, 10, "..#..."); accent_set(rows, 11, ".#...."); break;
      case ACC_CAP_ACUTE:  accent_set(rows, 0, "...#.."); break;
      case ACC_CAP_GRAVE:  accent_set(rows, 0, "..#..."); break;
    }

    {
      u8 *dst = &accent_font[accent_table[n].code * 9];
      bit = 0;
      for (r = 0; r < 12; r++)
        for (i = 0; i < 6; i++, bit++)
          if (rows[r] & (0x20 >> i))
            dst[bit >> 3] |= (u8)(0x80 >> (bit & 7));
    }
  }

  accent_font_ready = 1;
}

static s32 idx_hankaku(u8 ch);
static s32 idx_zenkaku(u8 hi, u8 lo);

static u8 iskanji_hi(u8 c);
static u8 iskanji_lo(u8 c);

/*****************************************************************************
* 文字をVRAMに転送
*****************************************************************************/
static void draw_char(s32 chr, u16 x, u16 y, u16 col, s16 bg_col, u16 *base_vram, u16 bufferwidth)
{
  u32 dx, dy;
  u16 *vptr0, *vptr;
  u8 bit, bitcnt;

  const u8 *font = shinonomefont;

  bitcnt = 0;
  bit = 0;

  if (chr >= ACCENT_IDX_BASE)
  {
    if (!accent_font_ready)
      accent_init();

    font = accent_font;
    chr -= ACCENT_IDX_BASE;
  }

  chr *= 9;

  vptr0 = base_vram + (x + (y * bufferwidth));

  for (dy = 0 ; dy < FONTHEIGHT ; dy++)
  {
    vptr = vptr0;

    for (dx = 0 ; dx < FONTWIDTH ; dx++)
    {
      bitcnt >>= 1;

      if (bitcnt == 0)
      {
        bit = font[chr++];
        bitcnt = 0x80;
      }

      if ((bit & bitcnt) != 0)
      {
        *vptr = col;
      }
      else if (bg_col >= 0)
      {
        *vptr = bg_col;
      }

      vptr++;
    }

    vptr0 += bufferwidth;
  }
}

/*****************************************************************************
* 半角文字のインデックスを取得
*****************************************************************************/
static s32 idx_hankaku(u8 ch)
{
  // 1 - 31 (accented letters of the French UI)
  if ((ch >= 1) && (ch < 0x20) && (ch != '\n'))
  {
    return ACCENT_IDX_BASE + ch;
  }

  // 0x20 - 0x7e (ASCII文字)
  if ((ch >= 0x20) && (ch <= 0x7e))
  {
    return (ch - 0x20);
  }

  // 0xA1 - 0xDF (半角カタカナ)
  if ((ch >= 0xA1) && (ch <= 0xDF))
  {
    return (ch - 0x40);
  }

  // 0xF0 - 0xF7 (Battery ICON), 0xF8 - 0xFA (GBA ICON)
  if ((ch >= 0xF0) && (ch <= 0xFD))
  {
    return (ch - 0x50);
  }

  return 0;
}

/*****************************************************************************
* 全角文字のインデックスを取得
*****************************************************************************/
static s32 idx_zenkaku(u8 hi, u8 lo)
{
  s32 idx;
  static const s32 codetbl[] =
  {
    -1,  0,  1,  2,  3, -1, -1,  4,  5,  6,  7,  8,  9, 10, 11, 12,  /* 0x80 */
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,  /* 0x90 */
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, -1, -1, 40, 41, -1   /* 0xe0 */
  };

  /* 上位バイトからインデックスを得る */
  idx = codetbl[hi & 0x3F];

  /* 表にない文字はスペースに変換 */
  if (idx == -1)
  {
    return (16 * 11);
  }

  idx = (16 * 11)           /* 半角文字をスキップ */
      + (idx * 2 * 192)     /* codetbl１個あたり192文字 */
      + ((lo - 0x40) << 1);

  return idx;
}

/*****************************************************************************
* 文字列の描画
*****************************************************************************/
void mh_print(const char *str, u16 x, u16 y, u16 col, s16 bg_col, u16 *base_vram, u16 bufferwidth)
{
  u16 dx, dy;
  u8 ch1, ch2;
  u8 bef = 0;
  s32 idx;

  dx = x;
  dy = y;

  while (*str != 0)
  {
    ch1 = *str++;
    ch2 = *str;

    if (ch1 == '\n')
    {
      dx = x;
      dy += FONTHEIGHT;
      bef = 0;
      continue;
    }

    if (bef != 0)
    {
      if (dx > (PSP_SCREEN_WIDTH - (FONTWIDTH * 2)))
        goto draw_end;

      idx = idx_zenkaku(bef, ch1);
      draw_char(idx,     dx,             dy, col, bg_col, base_vram, bufferwidth);
      draw_char(idx + 1, dx + FONTWIDTH, dy, col, bg_col, base_vram, bufferwidth);

      dx += (FONTWIDTH * 2);
      bef = 0;
    }
    else
    {
      if ((iskanji_hi(ch1) != 0) && (iskanji_lo(ch2) != 0))
      {
        bef = ch1;
      }
      else
      {
        if (dx > (PSP_SCREEN_WIDTH - FONTWIDTH))
          goto draw_end;

        idx = idx_hankaku(ch1);
        draw_char(idx, dx, dy, col, bg_col, base_vram, bufferwidth);

        dx += FONTWIDTH;
      }
    }
  } /* while */

  draw_end:;
}

/*****************************************************************************
* 全角文字の上位1バイトかどうかを判定  0x81～0x9F、0xe0～0xeF
*****************************************************************************/
static u8 iskanji_hi(u8 c)
{
  return (((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xe0) && (c <= 0xeF)));
}

/*****************************************************************************
* 全角文字の下位1バイトかどうかを判定  0x40～0x7e、 0x80～0xFC
*****************************************************************************/
static u8 iskanji_lo(u8 c)
{
  return (((c >= 0x40) && (c <= 0x7e)) || ((c >= 0x80) && (c <= 0xFC)));
}


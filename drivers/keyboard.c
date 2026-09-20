#include "devices.h"
#include "io.h"

static bool shift_down;
static bool caps_lock;
static bool extended;

/* The physical scan-code table follows the Turkish Q layout for ASCII keys.
 * Unicode-only Turkish letters are intentionally represented by ASCII fallbacks
 * because the stage-1 VGA text console is an 8-bit code-page surface. */
static const char normal[128] = {
    [0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',[0x07]='6',[0x08]='7',[0x09]='8',[0x0A]='9',[0x0B]='0',
    [0x0C]='-',[0x0D]='=',[0x0E]='\b',[0x0F]='\t',
    [0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',[0x15]='y',[0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',
    [0x1A]='g',[0x1B]='u',[0x1C]='\n',
    [0x1E]='a',[0x1F]='s',[0x20]='d',[0x21]='f',[0x22]='g',[0x23]='h',[0x24]='j',[0x25]='k',[0x26]='l',
    [0x27]='s',[0x28]='i',[0x29]='`',
    [0x2B]='\\',[0x2C]='z',[0x2D]='x',[0x2E]='c',[0x2F]='v',[0x30]='b',[0x31]='n',[0x32]='m',[0x33]='o',[0x34]='c',[0x35]='.',
    [0x39]=' ',
};
static const char shifted[128] = {
    [0x02]='!',[0x03]='"',[0x04]='^',[0x05]='+',[0x06]='%',[0x07]='&',[0x08]='/',[0x09]='(',[0x0A]=')',[0x0B]='=',
    [0x0C]='_',[0x0D]='*',[0x0E]='\b',[0x0F]='\t',
    [0x10]='Q',[0x11]='W',[0x12]='E',[0x13]='R',[0x14]='T',[0x15]='Y',[0x16]='U',[0x17]='I',[0x18]='O',[0x19]='P',
    [0x1A]='G',[0x1B]='U',[0x1C]='\n',
    [0x1E]='A',[0x1F]='S',[0x20]='D',[0x21]='F',[0x22]='G',[0x23]='H',[0x24]='J',[0x25]='K',[0x26]='L',
    [0x27]='S',[0x28]='I',[0x29]='~',
    [0x2B]='|',[0x2C]='Z',[0x2D]='X',[0x2E]='C',[0x2F]='V',[0x30]='B',[0x31]='N',[0x32]='M',[0x33]='O',[0x34]='C',[0x35]=':',
    [0x39]=' ',
};

void keyboard_init(void) {
    shift_down = false;
    caps_lock = false;
    extended = false;
}

bool keyboard_poll(char *out) {
    if (!(inb(0x64) & 0x01)) return false;
    uint8_t code = inb(0x60);
    if (code == 0xE0) { extended = true; return false; }
    if (code == 0x2A || code == 0x36) { shift_down = true; return false; }
    if (code == 0xAA || code == 0xB6) { shift_down = false; return false; }
    if (code == 0x3A && !extended) { caps_lock = !caps_lock; return false; }
    if (code & 0x80) { extended = false; return false; }
    extended = false;
    if (code >= 128) return false;
    char c = shift_down ? shifted[code] : normal[code];
    if (caps_lock && c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (!c) return false;
    *out = c;
    return true;
}

/****************************************************************************
**
** Copyright (C) 2015 WxMaper (http://wxmaper.ru)
**
** This file is part of the PQEngineCore extension of the PQEngine.
**
** BEGIN LICENSE: MPL 2.0
**
** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at http://mozilla.org/MPL/2.0/.
**
** END LICENSE
**
****************************************************************************/

#include "pqenginecore.h"
#include "pqclasses.h"

QMetaObjectList PQEngineCore::classes() {
    QMetaObjectList classes;

    classes << PQCoreApplication::staticMetaObject
            << PQEvent::staticMetaObject
            << PQLibrary::staticMetaObject
            << PQObject::staticMetaObject
            << PQRegExp::staticMetaObject
            << PQSettings::staticMetaObject
            << PQStandardPaths::staticMetaObject
            << PQTimer::staticMetaObject
            << PQThread::staticMetaObject;

    return classes;
}

bool PQEngineCore::start() {
    qRegisterMetaType<PQEvent*>("PQEvent*");
    qRegisterMetaType<PQObject*>("PQObject*");
    qRegisterMetaType<QThread*>("QThread*");
    qRegisterMetaType<PQThread*>("PQThread*");

    return true;
}

bool PQEngineCore::finalize() {
    /*
     * Константы Qt
     */
    pq_register_extra_zend_ce("Qt");

    /*
     * Клавиши
     */
    pq_register_long_constant_ex("Qt", "Key_Escape", Qt::Key_Escape, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Tab", Qt::Key_Tab, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Backtab", Qt::Key_Backtab, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Backspace", Qt::Key_Backspace, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Return", Qt::Key_Return, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Enter", Qt::Key_Enter, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Insert", Qt::Key_Insert, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Delete", Qt::Key_Delete, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Pause", Qt::Key_Pause, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Print", Qt::Key_Print, "Qt");
    pq_register_long_constant_ex("Qt", "Key_SysReq", Qt::Key_SysReq, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Clear", Qt::Key_Clear, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Home", Qt::Key_Home, "Qt");
    pq_register_long_constant_ex("Qt", "Key_End", Qt::Key_End, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Left", Qt::Key_Left, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Up", Qt::Key_Up, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Right", Qt::Key_Right, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Down", Qt::Key_Down, "Qt");
    pq_register_long_constant_ex("Qt", "Key_PageUp", Qt::Key_PageUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_PageDown", Qt::Key_PageDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Shift", Qt::Key_Shift, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Control", Qt::Key_Control, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Meta", Qt::Key_Meta, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Alt", Qt::Key_Alt, "Qt");
    pq_register_long_constant_ex("Qt", "Key_CapsLock", Qt::Key_CapsLock, "Qt");
    pq_register_long_constant_ex("Qt", "Key_NumLock", Qt::Key_NumLock, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ScrollLock", Qt::Key_ScrollLock, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F1", Qt::Key_F1, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F2", Qt::Key_F2, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F3", Qt::Key_F3, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F4", Qt::Key_F4, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F5", Qt::Key_F5, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F6", Qt::Key_F6, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F7", Qt::Key_F7, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F8", Qt::Key_F8, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F9", Qt::Key_F9, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F10", Qt::Key_F10, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F11", Qt::Key_F11, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F12", Qt::Key_F12, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F13", Qt::Key_F13, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F14", Qt::Key_F14, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F15", Qt::Key_F15, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F16", Qt::Key_F16, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F17", Qt::Key_F17, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F18", Qt::Key_F18, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F19", Qt::Key_F19, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F20", Qt::Key_F20, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F21", Qt::Key_F21, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F22", Qt::Key_F22, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F23", Qt::Key_F23, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F24", Qt::Key_F24, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F25", Qt::Key_F25, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F26", Qt::Key_F26, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F27", Qt::Key_F27, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F28", Qt::Key_F28, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F29", Qt::Key_F29, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F30", Qt::Key_F30, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F31", Qt::Key_F31, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F32", Qt::Key_F32, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F33", Qt::Key_F33, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F34", Qt::Key_F34, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F35", Qt::Key_F35, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Super_L", Qt::Key_Super_L, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Super_R", Qt::Key_Super_R, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Menu", Qt::Key_Menu, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hyper_L", Qt::Key_Hyper_L, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hyper_R", Qt::Key_Hyper_R, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Help", Qt::Key_Help, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Direction_L", Qt::Key_Direction_L, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Direction_R", Qt::Key_Direction_R, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Space", Qt::Key_Space, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Any", Qt::Key_Any, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Exclam", Qt::Key_Exclam, "Qt");
    pq_register_long_constant_ex("Qt", "Key_QuoteDbl", Qt::Key_QuoteDbl, "Qt");
    pq_register_long_constant_ex("Qt", "Key_NumberSign", Qt::Key_NumberSign, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dollar", Qt::Key_Dollar, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Percent", Qt::Key_Percent, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ampersand", Qt::Key_Ampersand, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Apostrophe", Qt::Key_Apostrophe, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ParenLeft", Qt::Key_ParenLeft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ParenRight", Qt::Key_ParenRight, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Asterisk", Qt::Key_Asterisk, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Plus", Qt::Key_Plus, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Comma", Qt::Key_Comma, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Minus", Qt::Key_Minus, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Period", Qt::Key_Period, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Slash", Qt::Key_Slash, "Qt");
    pq_register_long_constant_ex("Qt", "Key_0", Qt::Key_0, "Qt");
    pq_register_long_constant_ex("Qt", "Key_1", Qt::Key_1, "Qt");
    pq_register_long_constant_ex("Qt", "Key_2", Qt::Key_2, "Qt");
    pq_register_long_constant_ex("Qt", "Key_3", Qt::Key_3, "Qt");
    pq_register_long_constant_ex("Qt", "Key_4", Qt::Key_4, "Qt");
    pq_register_long_constant_ex("Qt", "Key_5", Qt::Key_5, "Qt");
    pq_register_long_constant_ex("Qt", "Key_6", Qt::Key_6, "Qt");
    pq_register_long_constant_ex("Qt", "Key_7", Qt::Key_7, "Qt");
    pq_register_long_constant_ex("Qt", "Key_8", Qt::Key_8, "Qt");
    pq_register_long_constant_ex("Qt", "Key_9", Qt::Key_9, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Colon", Qt::Key_Colon, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Semicolon", Qt::Key_Semicolon, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Less", Qt::Key_Less, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Equal", Qt::Key_Equal, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Greater", Qt::Key_Greater, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Question", Qt::Key_Question, "Qt");
    pq_register_long_constant_ex("Qt", "Key_At", Qt::Key_At, "Qt");
    pq_register_long_constant_ex("Qt", "Key_A", Qt::Key_A, "Qt");
    pq_register_long_constant_ex("Qt", "Key_B", Qt::Key_B, "Qt");
    pq_register_long_constant_ex("Qt", "Key_C", Qt::Key_C, "Qt");
    pq_register_long_constant_ex("Qt", "Key_D", Qt::Key_D, "Qt");
    pq_register_long_constant_ex("Qt", "Key_E", Qt::Key_E, "Qt");
    pq_register_long_constant_ex("Qt", "Key_F", Qt::Key_F, "Qt");
    pq_register_long_constant_ex("Qt", "Key_G", Qt::Key_G, "Qt");
    pq_register_long_constant_ex("Qt", "Key_H", Qt::Key_H, "Qt");
    pq_register_long_constant_ex("Qt", "Key_I", Qt::Key_I, "Qt");
    pq_register_long_constant_ex("Qt", "Key_J", Qt::Key_J, "Qt");
    pq_register_long_constant_ex("Qt", "Key_K", Qt::Key_K, "Qt");
    pq_register_long_constant_ex("Qt", "Key_L", Qt::Key_L, "Qt");
    pq_register_long_constant_ex("Qt", "Key_M", Qt::Key_M, "Qt");
    pq_register_long_constant_ex("Qt", "Key_N", Qt::Key_N, "Qt");
    pq_register_long_constant_ex("Qt", "Key_O", Qt::Key_O, "Qt");
    pq_register_long_constant_ex("Qt", "Key_P", Qt::Key_P, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Q", Qt::Key_Q, "Qt");
    pq_register_long_constant_ex("Qt", "Key_R", Qt::Key_R, "Qt");
    pq_register_long_constant_ex("Qt", "Key_S", Qt::Key_S, "Qt");
    pq_register_long_constant_ex("Qt", "Key_T", Qt::Key_T, "Qt");
    pq_register_long_constant_ex("Qt", "Key_U", Qt::Key_U, "Qt");
    pq_register_long_constant_ex("Qt", "Key_V", Qt::Key_V, "Qt");
    pq_register_long_constant_ex("Qt", "Key_W", Qt::Key_W, "Qt");
    pq_register_long_constant_ex("Qt", "Key_X", Qt::Key_X, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Y", Qt::Key_Y, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Z", Qt::Key_Z, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BracketLeft", Qt::Key_BracketLeft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Backslash", Qt::Key_Backslash, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BracketRight", Qt::Key_BracketRight, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AsciiCircum", Qt::Key_AsciiCircum, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Underscore", Qt::Key_Underscore, "Qt");
    pq_register_long_constant_ex("Qt", "Key_QuoteLeft", Qt::Key_QuoteLeft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BraceLeft", Qt::Key_BraceLeft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Bar", Qt::Key_Bar, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BraceRight", Qt::Key_BraceRight, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AsciiTilde", Qt::Key_AsciiTilde, "Qt");
    pq_register_long_constant_ex("Qt", "Key_nobreakspace", Qt::Key_nobreakspace, "Qt");
    pq_register_long_constant_ex("Qt", "Key_exclamdown", Qt::Key_exclamdown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_cent", Qt::Key_cent, "Qt");
    pq_register_long_constant_ex("Qt", "Key_sterling", Qt::Key_sterling, "Qt");
    pq_register_long_constant_ex("Qt", "Key_currency", Qt::Key_currency, "Qt");
    pq_register_long_constant_ex("Qt", "Key_yen", Qt::Key_yen, "Qt");
    pq_register_long_constant_ex("Qt", "Key_brokenbar", Qt::Key_brokenbar, "Qt");
    pq_register_long_constant_ex("Qt", "Key_section", Qt::Key_section, "Qt");
    pq_register_long_constant_ex("Qt", "Key_diaeresis", Qt::Key_diaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_copyright", Qt::Key_copyright, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ordfeminine", Qt::Key_ordfeminine, "Qt");
    pq_register_long_constant_ex("Qt", "Key_guillemotleft", Qt::Key_guillemotleft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_notsign", Qt::Key_notsign, "Qt");
    pq_register_long_constant_ex("Qt", "Key_hyphen", Qt::Key_hyphen, "Qt");
    pq_register_long_constant_ex("Qt", "Key_registered", Qt::Key_registered, "Qt");
    pq_register_long_constant_ex("Qt", "Key_macron", Qt::Key_macron, "Qt");
    pq_register_long_constant_ex("Qt", "Key_degree", Qt::Key_degree, "Qt");
    pq_register_long_constant_ex("Qt", "Key_plusminus", Qt::Key_plusminus, "Qt");
    pq_register_long_constant_ex("Qt", "Key_twosuperior", Qt::Key_twosuperior, "Qt");
    pq_register_long_constant_ex("Qt", "Key_threesuperior", Qt::Key_threesuperior, "Qt");
    pq_register_long_constant_ex("Qt", "Key_acute", Qt::Key_acute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_mu", Qt::Key_mu, "Qt");
    pq_register_long_constant_ex("Qt", "Key_paragraph", Qt::Key_paragraph, "Qt");
    pq_register_long_constant_ex("Qt", "Key_periodcentered", Qt::Key_periodcentered, "Qt");
    pq_register_long_constant_ex("Qt", "Key_cedilla", Qt::Key_cedilla, "Qt");
    pq_register_long_constant_ex("Qt", "Key_onesuperior", Qt::Key_onesuperior, "Qt");
    pq_register_long_constant_ex("Qt", "Key_masculine", Qt::Key_masculine, "Qt");
    pq_register_long_constant_ex("Qt", "Key_guillemotright", Qt::Key_guillemotright, "Qt");
    pq_register_long_constant_ex("Qt", "Key_onequarter", Qt::Key_onequarter, "Qt");
    pq_register_long_constant_ex("Qt", "Key_onehalf", Qt::Key_onehalf, "Qt");
    pq_register_long_constant_ex("Qt", "Key_threequarters", Qt::Key_threequarters, "Qt");
    pq_register_long_constant_ex("Qt", "Key_questiondown", Qt::Key_questiondown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Agrave", Qt::Key_Agrave, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Aacute", Qt::Key_Aacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Acircumflex", Qt::Key_Acircumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Atilde", Qt::Key_Atilde, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Adiaeresis", Qt::Key_Adiaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Aring", Qt::Key_Aring, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AE", Qt::Key_AE, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ccedilla", Qt::Key_Ccedilla, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Egrave", Qt::Key_Egrave, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Eacute", Qt::Key_Eacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ecircumflex", Qt::Key_Ecircumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ediaeresis", Qt::Key_Ediaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Igrave", Qt::Key_Igrave, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Iacute", Qt::Key_Iacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Icircumflex", Qt::Key_Icircumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Idiaeresis", Qt::Key_Idiaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ETH", Qt::Key_ETH, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ntilde", Qt::Key_Ntilde, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ograve", Qt::Key_Ograve, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Oacute", Qt::Key_Oacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ocircumflex", Qt::Key_Ocircumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Otilde", Qt::Key_Otilde, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Odiaeresis", Qt::Key_Odiaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_multiply", Qt::Key_multiply, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ooblique", Qt::Key_Ooblique, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ugrave", Qt::Key_Ugrave, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Uacute", Qt::Key_Uacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Ucircumflex", Qt::Key_Ucircumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Udiaeresis", Qt::Key_Udiaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Yacute", Qt::Key_Yacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_THORN", Qt::Key_THORN, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ssharp", Qt::Key_ssharp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_division", Qt::Key_division, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ydiaeresis", Qt::Key_ydiaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AltGr", Qt::Key_AltGr, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Multi_key", Qt::Key_Multi_key, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Codeinput", Qt::Key_Codeinput, "Qt");
    pq_register_long_constant_ex("Qt", "Key_SingleCandidate", Qt::Key_SingleCandidate, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MultipleCandidate", Qt::Key_MultipleCandidate, "Qt");
    pq_register_long_constant_ex("Qt", "Key_PreviousCandidate", Qt::Key_PreviousCandidate, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Mode_switch", Qt::Key_Mode_switch, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Kanji", Qt::Key_Kanji, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Muhenkan", Qt::Key_Muhenkan, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Henkan", Qt::Key_Henkan, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Romaji", Qt::Key_Romaji, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hiragana", Qt::Key_Hiragana, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Katakana", Qt::Key_Katakana, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hiragana_Katakana", Qt::Key_Hiragana_Katakana, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Zenkaku", Qt::Key_Zenkaku, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hankaku", Qt::Key_Hankaku, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Zenkaku_Hankaku", Qt::Key_Zenkaku_Hankaku, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Touroku", Qt::Key_Touroku, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Massyo", Qt::Key_Massyo, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Kana_Lock", Qt::Key_Kana_Lock, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Kana_Shift", Qt::Key_Kana_Shift, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Eisu_Shift", Qt::Key_Eisu_Shift, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Eisu_toggle", Qt::Key_Eisu_toggle, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul", Qt::Key_Hangul, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Start", Qt::Key_Hangul_Start, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_End", Qt::Key_Hangul_End, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Hanja", Qt::Key_Hangul_Hanja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Jamo", Qt::Key_Hangul_Jamo, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Romaja", Qt::Key_Hangul_Romaja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Jeonja", Qt::Key_Hangul_Jeonja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Banja", Qt::Key_Hangul_Banja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_PreHanja", Qt::Key_Hangul_PreHanja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_PostHanja", Qt::Key_Hangul_PostHanja, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangul_Special", Qt::Key_Hangul_Special, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Grave", Qt::Key_Dead_Grave, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Acute", Qt::Key_Dead_Acute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Circumflex", Qt::Key_Dead_Circumflex, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Tilde", Qt::Key_Dead_Tilde, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Macron", Qt::Key_Dead_Macron, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Breve", Qt::Key_Dead_Breve, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Abovedot", Qt::Key_Dead_Abovedot, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Diaeresis", Qt::Key_Dead_Diaeresis, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Abovering", Qt::Key_Dead_Abovering, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Doubleacute", Qt::Key_Dead_Doubleacute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Caron", Qt::Key_Dead_Caron, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Cedilla", Qt::Key_Dead_Cedilla, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Ogonek", Qt::Key_Dead_Ogonek, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Iota", Qt::Key_Dead_Iota, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Voiced_Sound", Qt::Key_Dead_Voiced_Sound, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Semivoiced_Sound", Qt::Key_Dead_Semivoiced_Sound, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Belowdot", Qt::Key_Dead_Belowdot, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Hook", Qt::Key_Dead_Hook, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Dead_Horn", Qt::Key_Dead_Horn, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Back", Qt::Key_Back, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Forward", Qt::Key_Forward, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Stop", Qt::Key_Stop, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Refresh", Qt::Key_Refresh, "Qt");
    pq_register_long_constant_ex("Qt", "Key_VolumeDown", Qt::Key_VolumeDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_VolumeMute", Qt::Key_VolumeMute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_VolumeUp", Qt::Key_VolumeUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BassBoost", Qt::Key_BassBoost, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BassUp", Qt::Key_BassUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BassDown", Qt::Key_BassDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TrebleUp", Qt::Key_TrebleUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TrebleDown", Qt::Key_TrebleDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaPlay", Qt::Key_MediaPlay, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaStop", Qt::Key_MediaStop, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaPrevious", Qt::Key_MediaPrevious, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaNext", Qt::Key_MediaNext, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaRecord", Qt::Key_MediaRecord, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaPause", Qt::Key_MediaPause, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaTogglePlayPause", Qt::Key_MediaTogglePlayPause, "Qt");
    pq_register_long_constant_ex("Qt", "Key_HomePage", Qt::Key_HomePage, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Favorites", Qt::Key_Favorites, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Search", Qt::Key_Search, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Standby", Qt::Key_Standby, "Qt");
    pq_register_long_constant_ex("Qt", "Key_OpenUrl", Qt::Key_OpenUrl, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchMail", Qt::Key_LaunchMail, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchMedia", Qt::Key_LaunchMedia, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch0", Qt::Key_Launch0, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch1", Qt::Key_Launch1, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch2", Qt::Key_Launch2, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch3", Qt::Key_Launch3, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch4", Qt::Key_Launch4, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch5", Qt::Key_Launch5, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch6", Qt::Key_Launch6, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch7", Qt::Key_Launch7, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch8", Qt::Key_Launch8, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Launch9", Qt::Key_Launch9, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchA", Qt::Key_LaunchA, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchB", Qt::Key_LaunchB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchC", Qt::Key_LaunchC, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchD", Qt::Key_LaunchD, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchE", Qt::Key_LaunchE, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchF", Qt::Key_LaunchF, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MonBrightnessUp", Qt::Key_MonBrightnessUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MonBrightnessDown", Qt::Key_MonBrightnessDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_KeyboardLightOnOff", Qt::Key_KeyboardLightOnOff, "Qt");
    pq_register_long_constant_ex("Qt", "Key_KeyboardBrightnessUp", Qt::Key_KeyboardBrightnessUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_KeyboardBrightnessDown", Qt::Key_KeyboardBrightnessDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_PowerOff", Qt::Key_PowerOff, "Qt");
    pq_register_long_constant_ex("Qt", "Key_WakeUp", Qt::Key_WakeUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Eject", Qt::Key_Eject, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ScreenSaver", Qt::Key_ScreenSaver, "Qt");
    pq_register_long_constant_ex("Qt", "Key_WWW", Qt::Key_WWW, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Memo", Qt::Key_Memo, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LightBulb", Qt::Key_LightBulb, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Shop", Qt::Key_Shop, "Qt");
    pq_register_long_constant_ex("Qt", "Key_History", Qt::Key_History, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AddFavorite", Qt::Key_AddFavorite, "Qt");
    pq_register_long_constant_ex("Qt", "Key_HotLinks", Qt::Key_HotLinks, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BrightnessAdjust", Qt::Key_BrightnessAdjust, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Finance", Qt::Key_Finance, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Community", Qt::Key_Community, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AudioRewind", Qt::Key_AudioRewind, "Qt");
    pq_register_long_constant_ex("Qt", "Key_BackForward", Qt::Key_BackForward, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ApplicationLeft", Qt::Key_ApplicationLeft, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ApplicationRight", Qt::Key_ApplicationRight, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Book", Qt::Key_Book, "Qt");
    pq_register_long_constant_ex("Qt", "Key_CD", Qt::Key_CD, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Calculator", Qt::Key_Calculator, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ToDoList", Qt::Key_ToDoList, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ClearGrab", Qt::Key_ClearGrab, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Close", Qt::Key_Close, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Copy", Qt::Key_Copy, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Cut", Qt::Key_Cut, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Display", Qt::Key_Display, "Qt");
    pq_register_long_constant_ex("Qt", "Key_DOS", Qt::Key_DOS, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Documents", Qt::Key_Documents, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Excel", Qt::Key_Excel, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Explorer", Qt::Key_Explorer, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Game", Qt::Key_Game, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Go", Qt::Key_Go, "Qt");
    pq_register_long_constant_ex("Qt", "Key_iTouch", Qt::Key_iTouch, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LogOff", Qt::Key_LogOff, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Market", Qt::Key_Market, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Meeting", Qt::Key_Meeting, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MenuKB", Qt::Key_MenuKB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MenuPB", Qt::Key_MenuPB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MySites", Qt::Key_MySites, "Qt");
    pq_register_long_constant_ex("Qt", "Key_News", Qt::Key_News, "Qt");
    pq_register_long_constant_ex("Qt", "Key_OfficeHome", Qt::Key_OfficeHome, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Option", Qt::Key_Option, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Paste", Qt::Key_Paste, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Phone", Qt::Key_Phone, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Calendar", Qt::Key_Calendar, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Reply", Qt::Key_Reply, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Reload", Qt::Key_Reload, "Qt");
    pq_register_long_constant_ex("Qt", "Key_RotateWindows", Qt::Key_RotateWindows, "Qt");
    pq_register_long_constant_ex("Qt", "Key_RotationPB", Qt::Key_RotationPB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_RotationKB", Qt::Key_RotationKB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Save", Qt::Key_Save, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Send", Qt::Key_Send, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Spell", Qt::Key_Spell, "Qt");
    pq_register_long_constant_ex("Qt", "Key_SplitScreen", Qt::Key_SplitScreen, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Support", Qt::Key_Support, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TaskPane", Qt::Key_TaskPane, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Terminal", Qt::Key_Terminal, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Tools", Qt::Key_Tools, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Travel", Qt::Key_Travel, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Video", Qt::Key_Video, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Word", Qt::Key_Word, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Xfer", Qt::Key_Xfer, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ZoomIn", Qt::Key_ZoomIn, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ZoomOut", Qt::Key_ZoomOut, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Away", Qt::Key_Away, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Messenger", Qt::Key_Messenger, "Qt");
    pq_register_long_constant_ex("Qt", "Key_WebCam", Qt::Key_WebCam, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MailForward", Qt::Key_MailForward, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Pictures", Qt::Key_Pictures, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Music", Qt::Key_Music, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Battery", Qt::Key_Battery, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Bluetooth", Qt::Key_Bluetooth, "Qt");
    pq_register_long_constant_ex("Qt", "Key_WLAN", Qt::Key_WLAN, "Qt");
    pq_register_long_constant_ex("Qt", "Key_UWB", Qt::Key_UWB, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AudioForward", Qt::Key_AudioForward, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AudioRepeat", Qt::Key_AudioRepeat, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AudioRandomPlay", Qt::Key_AudioRandomPlay, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Subtitle", Qt::Key_Subtitle, "Qt");
    pq_register_long_constant_ex("Qt", "Key_AudioCycleTrack", Qt::Key_AudioCycleTrack, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Time", Qt::Key_Time, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hibernate", Qt::Key_Hibernate, "Qt");
    pq_register_long_constant_ex("Qt", "Key_View", Qt::Key_View, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TopMenu", Qt::Key_TopMenu, "Qt");
    pq_register_long_constant_ex("Qt", "Key_PowerDown", Qt::Key_PowerDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Suspend", Qt::Key_Suspend, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ContrastAdjust", Qt::Key_ContrastAdjust, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchG", Qt::Key_LaunchG, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LaunchH", Qt::Key_LaunchH, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TouchpadToggle", Qt::Key_TouchpadToggle, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TouchpadOn", Qt::Key_TouchpadOn, "Qt");
    pq_register_long_constant_ex("Qt", "Key_TouchpadOff", Qt::Key_TouchpadOff, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MicMute", Qt::Key_MicMute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Red", Qt::Key_Red, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Green", Qt::Key_Green, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Yellow", Qt::Key_Yellow, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Blue", Qt::Key_Blue, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ChannelUp", Qt::Key_ChannelUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ChannelDown", Qt::Key_ChannelDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Guide", Qt::Key_Guide, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Info", Qt::Key_Info, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Settings", Qt::Key_Settings, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MicVolumeUp", Qt::Key_MicVolumeUp, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MicVolumeDown", Qt::Key_MicVolumeDown, "Qt");
    pq_register_long_constant_ex("Qt", "Key_New", Qt::Key_New, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Open", Qt::Key_Open, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Find", Qt::Key_Find, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Undo", Qt::Key_Undo, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Redo", Qt::Key_Redo, "Qt");
    pq_register_long_constant_ex("Qt", "Key_MediaLast", Qt::Key_MediaLast, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Select", Qt::Key_Select, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Yes", Qt::Key_Yes, "Qt");
    pq_register_long_constant_ex("Qt", "Key_No", Qt::Key_No, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Cancel", Qt::Key_Cancel, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Printer", Qt::Key_Printer, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Execute", Qt::Key_Execute, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Sleep", Qt::Key_Sleep, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Play", Qt::Key_Play, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Zoom", Qt::Key_Zoom, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Exit", Qt::Key_Exit, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Context1", Qt::Key_Context1, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Context2", Qt::Key_Context2, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Context3", Qt::Key_Context3, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Context4", Qt::Key_Context4, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Call", Qt::Key_Call, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Hangup", Qt::Key_Hangup, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Flip", Qt::Key_Flip, "Qt");
    pq_register_long_constant_ex("Qt", "Key_ToggleCallHangup", Qt::Key_ToggleCallHangup, "Qt");
    pq_register_long_constant_ex("Qt", "Key_VoiceDial", Qt::Key_VoiceDial, "Qt");
    pq_register_long_constant_ex("Qt", "Key_LastNumberRedial", Qt::Key_LastNumberRedial, "Qt");
    pq_register_long_constant_ex("Qt", "Key_Camera", Qt::Key_Camera, "Qt");
    pq_register_long_constant_ex("Qt", "Key_CameraFocus", Qt::Key_CameraFocus, "Qt");
    pq_register_long_constant_ex("Qt", "Key_unknown", Qt::Key_unknown, "Qt");

    pq_register_long_constant_ex("Qt", "AlignLeft", Qt::AlignLeft, "Qt");
    pq_register_long_constant_ex("Qt", "AlignRight", Qt::AlignRight, "Qt");
    pq_register_long_constant_ex("Qt", "AlignHCenter", Qt::AlignHCenter, "Qt");
    pq_register_long_constant_ex("Qt", "AlignJustify", Qt::AlignJustify, "Qt");
    pq_register_long_constant_ex("Qt", "AlignTop", Qt::AlignTop, "Qt");
    pq_register_long_constant_ex("Qt", "AlignBottom", Qt::AlignBottom, "Qt");
    pq_register_long_constant_ex("Qt", "AlignVCenter", Qt::AlignVCenter, "Qt");
    pq_register_long_constant_ex("Qt", "AlignBaseline", Qt::AlignBaseline, "Qt");
    pq_register_long_constant_ex("Qt", "AlignCenter", Qt::AlignCenter, "Qt");
    pq_register_long_constant_ex("Qt", "AlignAbsolute", Qt::AlignAbsolute, "Qt");
    pq_register_long_constant_ex("Qt", "AlignLeading", Qt::AlignLeading, "Qt");
    pq_register_long_constant_ex("Qt", "AlignTrailing", Qt::AlignTrailing, "Qt");

    /*
     * Qt::KeyboardModifiers
     */
    pq_register_long_constant_ex("Qt", "NoModifier", Qt::NoModifier, "Qt");
    pq_register_long_constant_ex("Qt", "ShiftModifier", Qt::ShiftModifier, "Qt");
    pq_register_long_constant_ex("Qt", "ControlModifier", Qt::ControlModifier, "Qt");
    pq_register_long_constant_ex("Qt", "AltModifier", Qt::AltModifier, "Qt");
    pq_register_long_constant_ex("Qt", "MetaModifier", Qt::MetaModifier, "Qt");
    pq_register_long_constant_ex("Qt", "KeypadModifier", Qt::KeypadModifier, "Qt");
    pq_register_long_constant_ex("Qt", "GroupSwitchModifier", Qt::GroupSwitchModifier, "Qt");

    /*
     * Qt::TextInteractionFlags
     */
    pq_register_long_constant_ex("Qt", "NoTextInteraction", Qt::NoTextInteraction, "Qt");
    pq_register_long_constant_ex("Qt", "TextSelectableByMouse", Qt::TextSelectableByMouse, "Qt");
    pq_register_long_constant_ex("Qt", "TextSelectableByKeyboard", Qt::TextSelectableByKeyboard, "Qt");
    pq_register_long_constant_ex("Qt", "LinksAccessibleByMouse", Qt::LinksAccessibleByMouse, "Qt");
    pq_register_long_constant_ex("Qt", "LinksAccessibleByKeyboard", Qt::LinksAccessibleByKeyboard, "Qt");
    pq_register_long_constant_ex("Qt", "TextEditable", Qt::TextEditable, "Qt");
    pq_register_long_constant_ex("Qt", "TextEditorInteraction", Qt::TextEditorInteraction, "Qt");
    pq_register_long_constant_ex("Qt", "TextBrowserInteraction", Qt::TextBrowserInteraction, "Qt");

    /*
     * Константы QEvent
     */
    //pq_register_qevent_zend_ce("QEvent");
    pq_register_long_constant("QEvent", "ActionAdded", QEvent::ActionAdded);
    pq_register_long_constant("QEvent", "ActionChanged", QEvent::ActionChanged);
    pq_register_long_constant("QEvent", "ActionRemoved", QEvent::ActionRemoved);
    pq_register_long_constant("QEvent", "ActivationChange", QEvent::ActivationChange);
    pq_register_long_constant("QEvent", "ApplicationActivate", QEvent::ApplicationActivate);
    pq_register_long_constant("QEvent", "ApplicationActivated", QEvent::ApplicationActivated);
    pq_register_long_constant("QEvent", "ApplicationDeactivate", QEvent::ApplicationDeactivate);
    pq_register_long_constant("QEvent", "ApplicationFontChange", QEvent::ApplicationFontChange);
    pq_register_long_constant("QEvent", "ApplicationLayoutDirectionChange", QEvent::ApplicationLayoutDirectionChange);
    pq_register_long_constant("QEvent", "ApplicationPaletteChange", QEvent::ApplicationPaletteChange);
    pq_register_long_constant("QEvent", "ApplicationStateChange", QEvent::ApplicationStateChange);
    pq_register_long_constant("QEvent", "ApplicationWindowIconChange", QEvent::ApplicationWindowIconChange);
    pq_register_long_constant("QEvent", "ChildAdded", QEvent::ChildAdded);
    pq_register_long_constant("QEvent", "ChildPolished", QEvent::ChildPolished);
    pq_register_long_constant("QEvent", "ChildRemoved", QEvent::ChildRemoved);
    pq_register_long_constant("QEvent", "Clipboard", QEvent::Clipboard);
    pq_register_long_constant("QEvent", "Close", QEvent::Close);
    pq_register_long_constant("QEvent", "CloseSoftwareInputPanel", QEvent::CloseSoftwareInputPanel);
    pq_register_long_constant("QEvent", "ContentsRectChange", QEvent::ContentsRectChange);
    pq_register_long_constant("QEvent", "ContextMenu", QEvent::ContextMenu);
    pq_register_long_constant("QEvent", "CursorChange", QEvent::CursorChange);
    pq_register_long_constant("QEvent", "DeferredDelete", QEvent::DeferredDelete);
    pq_register_long_constant("QEvent", "DragEnter", QEvent::DragEnter);
    pq_register_long_constant("QEvent", "DragLeave", QEvent::DragLeave);
    pq_register_long_constant("QEvent", "DragMove", QEvent::DragMove);
    pq_register_long_constant("QEvent", "Drop", QEvent::Drop);
    pq_register_long_constant("QEvent", "DynamicPropertyChange", QEvent::DynamicPropertyChange);
    pq_register_long_constant("QEvent", "EnabledChange", QEvent::EnabledChange);
    pq_register_long_constant("QEvent", "Enter", QEvent::Enter);
    //pq_register_long_constant("QEvent", "EnterEditFocus", QEvent::EnterEditFocus);
    pq_register_long_constant("QEvent", "EnterWhatsThisMode", QEvent::EnterWhatsThisMode);
    pq_register_long_constant("QEvent", "Expose", QEvent::Expose);
    pq_register_long_constant("QEvent", "FileOpen", QEvent::FileOpen);
    pq_register_long_constant("QEvent", "FocusIn", QEvent::FocusIn);
    pq_register_long_constant("QEvent", "FocusOut", QEvent::FocusOut);
    pq_register_long_constant("QEvent", "FocusAboutToChange", QEvent::FocusAboutToChange);
    pq_register_long_constant("QEvent", "FontChange", QEvent::FontChange);
    pq_register_long_constant("QEvent", "Gesture", QEvent::Gesture);
    pq_register_long_constant("QEvent", "GestureOverride", QEvent::GestureOverride);
    pq_register_long_constant("QEvent", "GrabKeyboard", QEvent::GrabKeyboard);
    pq_register_long_constant("QEvent", "GrabMouse", QEvent::GrabMouse);
    pq_register_long_constant("QEvent", "GraphicsSceneContextMenu", QEvent::GraphicsSceneContextMenu);
    pq_register_long_constant("QEvent", "GraphicsSceneDragEnter", QEvent::GraphicsSceneDragEnter);
    pq_register_long_constant("QEvent", "GraphicsSceneDragLeave", QEvent::GraphicsSceneDragLeave);
    pq_register_long_constant("QEvent", "GraphicsSceneDragMove", QEvent::GraphicsSceneDragMove);
    pq_register_long_constant("QEvent", "GraphicsSceneDrop", QEvent::GraphicsSceneDrop);
    pq_register_long_constant("QEvent", "GraphicsSceneHelp", QEvent::GraphicsSceneHelp);
    pq_register_long_constant("QEvent", "GraphicsSceneHoverEnter", QEvent::GraphicsSceneHoverEnter);
    pq_register_long_constant("QEvent", "GraphicsSceneHoverLeave", QEvent::GraphicsSceneHoverLeave);
    pq_register_long_constant("QEvent", "GraphicsSceneHoverMove", QEvent::GraphicsSceneHoverMove);
    pq_register_long_constant("QEvent", "GraphicsSceneMouseDoubleClick", QEvent::GraphicsSceneMouseDoubleClick);
    pq_register_long_constant("QEvent", "GraphicsSceneMouseMove", QEvent::GraphicsSceneMouseMove);
    pq_register_long_constant("QEvent", "GraphicsSceneMousePress", QEvent::GraphicsSceneMousePress);
    pq_register_long_constant("QEvent", "GraphicsSceneMouseRelease", QEvent::GraphicsSceneMouseRelease);
    pq_register_long_constant("QEvent", "GraphicsSceneMove", QEvent::GraphicsSceneMove);
    pq_register_long_constant("QEvent", "GraphicsSceneResize", QEvent::GraphicsSceneResize);
    pq_register_long_constant("QEvent", "GraphicsSceneWheel", QEvent::GraphicsSceneWheel);
    pq_register_long_constant("QEvent", "Hide", QEvent::Hide);
    pq_register_long_constant("QEvent", "HideToParent", QEvent::HideToParent);
    pq_register_long_constant("QEvent", "HoverEnter", QEvent::HoverEnter);
    pq_register_long_constant("QEvent", "HoverLeave", QEvent::HoverLeave);
    pq_register_long_constant("QEvent", "HoverMove", QEvent::HoverMove);
    pq_register_long_constant("QEvent", "IconDrag", QEvent::IconDrag);
    pq_register_long_constant("QEvent", "IconTextChange", QEvent::IconTextChange);
    pq_register_long_constant("QEvent", "InputMethod", QEvent::InputMethod);
    pq_register_long_constant("QEvent", "InputMethodQuery", QEvent::InputMethodQuery);
    pq_register_long_constant("QEvent", "KeyboardLayoutChange", QEvent::KeyboardLayoutChange);
    pq_register_long_constant("QEvent", "KeyPress", QEvent::KeyPress);
    pq_register_long_constant("QEvent", "KeyRelease", QEvent::KeyRelease);
    pq_register_long_constant("QEvent", "LanguageChange", QEvent::LanguageChange);
    pq_register_long_constant("QEvent", "LayoutDirectionChange", QEvent::LayoutDirectionChange);
    pq_register_long_constant("QEvent", "LayoutRequest", QEvent::LayoutRequest);
    pq_register_long_constant("QEvent", "Leave", QEvent::Leave);
    //pq_register_long_constant("QEvent", "LeaveEditFocus", QEvent::LeaveEditFocus);
    pq_register_long_constant("QEvent", "LeaveWhatsThisMode", QEvent::LeaveWhatsThisMode);
    pq_register_long_constant("QEvent", "LocaleChange", QEvent::LocaleChange);
    pq_register_long_constant("QEvent", "NonClientAreaMouseButtonDblClick", QEvent::NonClientAreaMouseButtonDblClick);
    pq_register_long_constant("QEvent", "NonClientAreaMouseButtonPress", QEvent::NonClientAreaMouseButtonPress);
    pq_register_long_constant("QEvent", "NonClientAreaMouseButtonRelease", QEvent::NonClientAreaMouseButtonRelease);
    pq_register_long_constant("QEvent", "NonClientAreaMouseMove", QEvent::NonClientAreaMouseMove);
    pq_register_long_constant("QEvent", "MacSizeChange", QEvent::MacSizeChange);
    pq_register_long_constant("QEvent", "MetaCall", QEvent::MetaCall);
    pq_register_long_constant("QEvent", "ModifiedChange", QEvent::ModifiedChange);
    pq_register_long_constant("QEvent", "MouseButtonDblClick", QEvent::MouseButtonDblClick);
    pq_register_long_constant("QEvent", "MouseButtonPress", QEvent::MouseButtonPress);
    pq_register_long_constant("QEvent", "MouseButtonRelease", QEvent::MouseButtonRelease);
    pq_register_long_constant("QEvent", "MouseMove", QEvent::MouseMove);
    pq_register_long_constant("QEvent", "MouseTrackingChange", QEvent::MouseTrackingChange);
    pq_register_long_constant("QEvent", "Move", QEvent::Move);
    pq_register_long_constant("QEvent", "NativeGesture", QEvent::NativeGesture);
    pq_register_long_constant("QEvent", "OrientationChange", QEvent::OrientationChange);
    pq_register_long_constant("QEvent", "Paint", QEvent::Paint);
    pq_register_long_constant("QEvent", "PaletteChange", QEvent::PaletteChange);
    pq_register_long_constant("QEvent", "ParentAboutToChange", QEvent::ParentAboutToChange);
    pq_register_long_constant("QEvent", "ParentChange", QEvent::ParentChange);
    pq_register_long_constant("QEvent", "PlatformPanel", QEvent::PlatformPanel);
    pq_register_long_constant("QEvent", "PlatformSurface", QEvent::PlatformSurface);
    pq_register_long_constant("QEvent", "Polish", QEvent::Polish);
    pq_register_long_constant("QEvent", "PolishRequest", QEvent::PolishRequest);
    pq_register_long_constant("QEvent", "QueryWhatsThis", QEvent::QueryWhatsThis);
    pq_register_long_constant("QEvent", "ReadOnlyChange", QEvent::ReadOnlyChange);
    pq_register_long_constant("QEvent", "RequestSoftwareInputPanel", QEvent::RequestSoftwareInputPanel);
    pq_register_long_constant("QEvent", "Resize", QEvent::Resize);
    pq_register_long_constant("QEvent", "ScrollPrepare", QEvent::ScrollPrepare);
    pq_register_long_constant("QEvent", "Scroll", QEvent::Scroll);
    pq_register_long_constant("QEvent", "Shortcut", QEvent::Shortcut);
    pq_register_long_constant("QEvent", "ShortcutOverride", QEvent::ShortcutOverride);
    pq_register_long_constant("QEvent", "Show", QEvent::Show);
    pq_register_long_constant("QEvent", "ShowToParent", QEvent::ShowToParent);
    pq_register_long_constant("QEvent", "SockAct", QEvent::SockAct);
    pq_register_long_constant("QEvent", "StateMachineSignal", QEvent::StateMachineSignal);
    pq_register_long_constant("QEvent", "StateMachineWrapped", QEvent::StateMachineWrapped);
    pq_register_long_constant("QEvent", "StatusTip", QEvent::StatusTip);
    pq_register_long_constant("QEvent", "StyleChange", QEvent::StyleChange);
    pq_register_long_constant("QEvent", "TabletMove", QEvent::TabletMove);
    pq_register_long_constant("QEvent", "TabletPress", QEvent::TabletPress);
    pq_register_long_constant("QEvent", "TabletRelease", QEvent::TabletRelease);
    pq_register_long_constant("QEvent", "OkRequest", QEvent::OkRequest);
    pq_register_long_constant("QEvent", "TabletEnterProximity", QEvent::TabletEnterProximity);
    pq_register_long_constant("QEvent", "TabletLeaveProximity", QEvent::TabletLeaveProximity);
    pq_register_long_constant("QEvent", "ThreadChange", QEvent::ThreadChange);
    pq_register_long_constant("QEvent", "Timer", QEvent::Timer);
    pq_register_long_constant("QEvent", "ToolBarChange", QEvent::ToolBarChange);
    pq_register_long_constant("QEvent", "ToolTip", QEvent::ToolTip);
    pq_register_long_constant("QEvent", "ToolTipChange", QEvent::ToolTipChange);
    pq_register_long_constant("QEvent", "TouchBegin", QEvent::TouchBegin);
    pq_register_long_constant("QEvent", "TouchCancel", QEvent::TouchCancel);
    pq_register_long_constant("QEvent", "TouchEnd", QEvent::TouchEnd);
    pq_register_long_constant("QEvent", "TouchUpdate", QEvent::TouchUpdate);
    pq_register_long_constant("QEvent", "UngrabKeyboard", QEvent::UngrabKeyboard);
    pq_register_long_constant("QEvent", "UngrabMouse", QEvent::UngrabMouse);
    pq_register_long_constant("QEvent", "UpdateLater", QEvent::UpdateLater);
    pq_register_long_constant("QEvent", "UpdateRequest", QEvent::UpdateRequest);
    pq_register_long_constant("QEvent", "WhatsThis", QEvent::WhatsThis);
    pq_register_long_constant("QEvent", "WhatsThisClicked", QEvent::WhatsThisClicked);
    pq_register_long_constant("QEvent", "Wheel", QEvent::Wheel);
    pq_register_long_constant("QEvent", "WinEventAct", QEvent::WinEventAct);
    pq_register_long_constant("QEvent", "WindowActivate", QEvent::WindowActivate);
    pq_register_long_constant("QEvent", "WindowBlocked", QEvent::WindowBlocked);
    pq_register_long_constant("QEvent", "WindowDeactivate", QEvent::WindowDeactivate);
    pq_register_long_constant("QEvent", "WindowIconChange", QEvent::WindowIconChange);
    pq_register_long_constant("QEvent", "WindowStateChange", QEvent::WindowStateChange);
    pq_register_long_constant("QEvent", "WindowTitleChange", QEvent::WindowTitleChange);
    pq_register_long_constant("QEvent", "WindowUnblocked", QEvent::WindowUnblocked);
    pq_register_long_constant("QEvent", "WinIdChange", QEvent::WinIdChange);
    pq_register_long_constant("QEvent", "ZOrderChange", QEvent::ZOrderChange);


    /*
    pq_register_extra_zend_ce("QEvent");
    PQ_REGISTER_ZEND_CE(QEvent);
    PQ_REGISTER_ZEND_CE(QMouseEvent);
    PQ_REGISTER_ZEND_CE(QKeyEvent);
    PQ_REGISTER_ZEND_CE(QResizeEvent);
    PQ_REGISTER_ZEND_CE(QMoveEvent);
    PQ_REGISTER_ZEND_CE(QHoverEvent);
    PQ_REGISTER_ZEND_CE(QFocusEvent);
    PQ_REGISTER_ZEND_CE(QEnterEvent);
    PQ_REGISTER_ZEND_CE(QContextMenuEvent);
    PQ_REGISTER_ZEND_CE(QTimerEvent);
    */

    /*
     * Константы QSettings
     */
    pq_register_long_constant("QSettings", "NativeFormat", QSettings::NativeFormat);
    pq_register_long_constant("QSettings", "IniFormat", QSettings::IniFormat);
    pq_register_long_constant("QSettings", "InvalidFormat", QSettings::InvalidFormat);
    pq_register_long_constant("QSettings", "UserScope", QSettings::UserScope);
    pq_register_long_constant("QSettings", "SystemScope", QSettings::SystemScope);
    pq_register_long_constant("QSettings", "NoError", QSettings::NoError);
    pq_register_long_constant("QSettings", "AccessError", QSettings::AccessError);
    pq_register_long_constant("QSettings", "FormatError", QSettings::FormatError);


    /*
     * Константы QStandardPaths
     */
    pq_register_long_constant("QStandardPaths", "LocateFile", QStandardPaths::LocateFile);
    pq_register_long_constant("QStandardPaths", "LocateDirectory", QStandardPaths::LocateDirectory);
    pq_register_long_constant("QStandardPaths", "DesktopLocation", QStandardPaths::DesktopLocation);
    pq_register_long_constant("QStandardPaths", "DocumentsLocation", QStandardPaths::DocumentsLocation);
    pq_register_long_constant("QStandardPaths", "FontsLocation", QStandardPaths::FontsLocation);
    pq_register_long_constant("QStandardPaths", "ApplicationsLocation", QStandardPaths::ApplicationsLocation);
    pq_register_long_constant("QStandardPaths", "MusicLocation", QStandardPaths::MusicLocation);
    pq_register_long_constant("QStandardPaths", "MoviesLocation", QStandardPaths::MoviesLocation);
    pq_register_long_constant("QStandardPaths", "PicturesLocation", QStandardPaths::PicturesLocation);
    pq_register_long_constant("QStandardPaths", "TempLocation", QStandardPaths::TempLocation);
    pq_register_long_constant("QStandardPaths", "HomeLocation", QStandardPaths::HomeLocation);
    pq_register_long_constant("QStandardPaths", "DataLocation", QStandardPaths::DataLocation);
    pq_register_long_constant("QStandardPaths", "CacheLocation", QStandardPaths::CacheLocation);
    pq_register_long_constant("QStandardPaths", "GenericCacheLocation", QStandardPaths::GenericCacheLocation);
    pq_register_long_constant("QStandardPaths", "GenericDataLocation", QStandardPaths::GenericDataLocation);
    pq_register_long_constant("QStandardPaths", "RuntimeLocation", QStandardPaths::RuntimeLocation);
    pq_register_long_constant("QStandardPaths", "ConfigLocation", QStandardPaths::ConfigLocation);
    pq_register_long_constant("QStandardPaths", "DownloadLocation", QStandardPaths::DownloadLocation);
    pq_register_long_constant("QStandardPaths", "GenericConfigLocation", QStandardPaths::GenericConfigLocation);
    pq_register_long_constant("QStandardPaths", "AppDataLocation", QStandardPaths::AppDataLocation);
    pq_register_long_constant("QStandardPaths", "AppLocalDataLocation", QStandardPaths::AppLocalDataLocation);
    pq_register_long_constant("QStandardPaths", "AppConfigLocation", QStandardPaths::AppConfigLocation);

    pq_register_long_constant("QLibrary", "ResolveAllSymbolsHint", QLibrary::ResolveAllSymbolsHint);
    pq_register_long_constant("QLibrary", "ExportExternalSymbolsHint", QLibrary::ExportExternalSymbolsHint);
    pq_register_long_constant("QLibrary", "LoadArchiveMemberHint", QLibrary::LoadArchiveMemberHint);
    pq_register_long_constant("QLibrary", "PreventUnloadHint", QLibrary::PreventUnloadHint);
    pq_register_long_constant("QLibrary", "DeepBindHint", QLibrary::DeepBindHint);

    return true;
}

QCoreApplication *PQEngineCore::instance(int argc, char** argv) {
    return QCoreApplication::instance()
            ? QCoreApplication::instance()
            : new PQCoreApplication(argc, argv);
}

void PQEngineCore::ub_write(const QString &msg)
{
    default_ub_write(msg, "");
}

void PQEngineCore::pre(const QString &msg, const QString &title)
{
    default_ub_write(msg, title);
}

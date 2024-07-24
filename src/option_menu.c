#include "global.h"
#include "option_menu.h"
#include "main.h"
#include "menu.h"
#include "scanline_effect.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "text.h"
#include "text_window.h"
#include "international_string_util.h"
#include "string_util.h"

#define WINDOW_FRAMES_COUNT_FR 10
#define MENUITEM_DESCRIPTION MENUITEM_COUNT

extern void SetPokemonCryStereo(u32 val);

// Task data
enum
{
    TD_MENUSELECTION,
    TD_TEXTSPEED,
    TD_BATTLESCENE,
    TD_BATTLESTYLE,
    TD_SOUND,
    TD_BUTTONMODE,
    TD_FRAMETYPE,
	TD_TIMER,
};

// Menu items
enum
{
    MENUITEM_TEXTSPEED,
    MENUITEM_BATTLESCENE,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_COUNT,
};

// this file's functions
static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuCancel(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static u8   TextSpeed_ProcessInput(u8 selection);
static void TextSpeed_DrawChoices(u8 selection);
static u8   BattleScene_ProcessInput(u8 selection);
static void BattleScene_DrawChoices(u8 selection);
static u8   BattleStyle_ProcessInput(u8 selection);
static void BattleStyle_DrawChoices(u8 selection);
static u8   Sound_ProcessInput(u8 selection);
static void Sound_DrawChoices(u8 selection);
static u8   FrameType_ProcessInput(u8 selection);
static void FrameType_DrawChoices(u8 selection);
static u8   ButtonMode_ProcessInput(u8 selection);
static void ButtonMode_DrawChoices(u8 selection);
static void DrawOptionMenuTexts(void);
static void DrawFrame(void);
static void OptionMenu_ClearWindow(u8 option);
static void ShowDescription(const u8 *text);
static void SetDescription(u8 selection);

// EWRAM vars
u8 sArrowPressed = FALSE;

// const rom data
const u16 gPalOptionMenu[] = INCBIN_U16("graphics/option_menu/option_menu_text.gbapal");
const u32 gTilesOptionMenu[] = INCBIN_U32("graphics/option_menu/option_menu_tiles.4bpp.lz");
const u32 gMapOptionMenu0[] = INCBIN_U32("graphics/option_menu/option_menu_0.bin.lz");
const u32 gMapOptionMenu1[] = INCBIN_U32("graphics/option_menu/option_menu_1.bin.lz");
const u32 gMapOptionMenu2[] = INCBIN_U32("graphics/option_menu/option_menu_2.bin.lz");
const u32 gMapOptionMenu3[] = INCBIN_U32("graphics/option_menu/option_menu_3.bin.lz");
const u32 gMapOptionMenu4[] = INCBIN_U32("graphics/option_menu/option_menu_4.bin.lz");
const u32 gMapOptionMenu5[] = INCBIN_U32("graphics/option_menu/option_menu_5.bin.lz");
const u8 localText_Option[] = _("{COLOR 2}{HIGHLIGHT TRANSPARENT}OPTIONS");
const u8 localText_Instructions[] = _("{COLOR 2}{HIGHLIGHT TRANSPARENT}{SIZE 0}SAVE    CANCEL");
const u8 localText_TextSpeed[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Text Speed");
const u8 localText_BattleScene[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}BATTLE SCENE");
const u8 localText_BattleStyle[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}BATTLE STYLE");
const u8 localText_Sound[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}SOUND");
const u8 localText_Frame[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}FRAME");
const u8 localText_ButtonMode[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}CONTROLS");
const u8 localText_TextSpeedSlow[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}SLOW");
const u8 localText_TextSpeedMid[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}MID");
const u8 localText_TextSpeedFast[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}FAST");
const u8 localText_BattleSceneOn[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}ON");
const u8 localText_BattleSceneOff[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}OFF");
const u8 localText_BattleStyleShift[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}SHIFT");
const u8 localText_BattleStyleSet[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}SET");
const u8 localText_SoundMono[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}MONO");
const u8 localText_SoundStereo[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}STEREO");
const u8 localText_FrameType[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}TYPE");
const u8 localText_FrameTypeNumber[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}");
const u8 localText_ButtonTypeNormal[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}NORMAL");
const u8 localText_ButtonTypeLR[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}LR");
const u8 localText_ButtonTypeLEqualsA[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}L=A");
const u8 localText_TextSpeedDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Choose from three text speed levels.");
const u8 localText_BattleSceneDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Show or disable animations\nduring battles.");
const u8 localText_BattleStyleDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Configure the rules that apply in\ncombat.");
const u8 localText_SoundDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Set the sound to mono or stereo. ");
const u8 localText_FrameDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Configure menu borders.");
const u8 localText_ButtonModeDescription[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Configures the behavior of the\nL and R buttons.");
const u8 localText_ExitWithSave[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Saving the changes made...");
const u8 localText_ExitWithoutSave[] = _("{COLOR 1}{HIGHLIGHT TRANSPARENT}Discarding the changes made...");

static const u8 *const sOptionMenuItemsNames[MENUITEM_COUNT] =
{
    localText_TextSpeed,
    localText_BattleScene,
    localText_BattleStyle,
    localText_Sound,
    localText_ButtonMode,
    localText_Frame,
};

static const u8 *const sOptionMenuDescriptions[] =
{
    localText_TextSpeedDescription,
    localText_BattleSceneDescription,
    localText_BattleStyleDescription,
    localText_SoundDescription,
    localText_ButtonModeDescription,
    localText_FrameDescription,
	localText_ExitWithSave,
	localText_ExitWithoutSave,
};

static const u32 *const sOptionMenuItems[MENUITEM_COUNT] =
{
    gMapOptionMenu0,
    gMapOptionMenu1,
    gMapOptionMenu2,
    gMapOptionMenu3,
    gMapOptionMenu4,
    gMapOptionMenu5,
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 20,
        .paletteNum = 5,
        .baseBlock = 0
    },
	{
        .bg = 2,
        .tilemapLeft = 0,
        .tilemapTop = 14,
        .width = 30,
        .height = 6,
        .paletteNum = 14,
        .baseBlock = 0
    },
	DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
   {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
       .baseTile = 0	   
   },
   {
       .bg = 1,
       .charBaseIndex = 0,
       .mapBaseIndex = 7,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
       .baseTile = 0
   },
   {
       .bg = 2,
       .charBaseIndex = 0,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
       .baseTile = 0
   }
};

enum
{
	DESCRIPTION,
	FRAME,
};

// code
static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_NewInitOptionMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
		SetHBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
		DmaClearLarge16(3, (void*)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
		ResetBgsAndClearDma3BusyFlags(0);
		InitBgsFromTemplates(0, sOptionMenuBgTemplates, ARRAY_COUNT(sOptionMenuBgTemplates));
        ChangeBgX(0, 0, 0);
        ChangeBgY(0, 0, 0);
        ChangeBgX(1, 0, 0);
		ChangeBgY(1, 0, 0);
        ChangeBgX(2, 0, 0);
        ChangeBgY(2, 0, 0);
        ChangeBgX(3, 0, 0);
        ChangeBgY(3, 0, 0);
    	InitWindows(sOptionMenuWinTemplates);
		DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
	    ShowBg(0);
        ShowBg(1);
		ShowBg(2);
		gMain.state++;
        break;
    case 2:
        ResetSpriteData();
		ResetPaletteFade();
		FreeAllSpritePalettes();
     	ResetTasks();
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(2, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
		gMain.state++;
        break;
    case 4:
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0xE0, 0x20);
        gMain.state++;
        break;
    case 5:
		LZ77UnCompVram(gTilesOptionMenu, (void *)VRAM);
        gMain.state++;
        break;
    case 6:
		LZ77UnCompVram(gMapOptionMenu0, (void *)(VRAM + 0x3800)); 
        gMain.state++;
        break;
    case 7:
		LoadPalette(gPalOptionMenu, 0x50, 0xE0);
        gMain.state++;
        break;
    case 8:
		PutWindowTilemap(0);
		DrawOptionMenuTexts();
        gMain.state++;
        break;
    case 9:
		PutWindowTilemap(1);
		DrawFrame();
        gMain.state++;
        break;
    case 10:
    {
        u8 taskId = CreateTask(Task_OptionMenuFadeIn, 0);

        gTasks[taskId].data[TD_MENUSELECTION] = 0;
        gTasks[taskId].data[TD_TEXTSPEED] = gSaveBlock2Ptr->optionsTextSpeed;
        gTasks[taskId].data[TD_BATTLESCENE] = gSaveBlock2Ptr->optionsBattleSceneOff;
        gTasks[taskId].data[TD_BATTLESTYLE] = gSaveBlock2Ptr->optionsBattleStyle;
        gTasks[taskId].data[TD_SOUND] = gSaveBlock2Ptr->optionsSound;
        gTasks[taskId].data[TD_BUTTONMODE] = gSaveBlock2Ptr->optionsButtonMode;
        gTasks[taskId].data[TD_FRAMETYPE] = gSaveBlock2Ptr->optionsWindowFrameType;

        TextSpeed_DrawChoices(gTasks[taskId].data[TD_TEXTSPEED]);
        BattleScene_DrawChoices(gTasks[taskId].data[TD_BATTLESCENE]);
        BattleStyle_DrawChoices(gTasks[taskId].data[TD_BATTLESTYLE]);
        Sound_DrawChoices(gTasks[taskId].data[TD_SOUND]);
        ButtonMode_DrawChoices(gTasks[taskId].data[TD_BUTTONMODE]);
        FrameType_DrawChoices(gTasks[taskId].data[TD_FRAMETYPE]);
        
		CopyWindowToVram(0, 3);
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0x10, 0, 0);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
		return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
	if (gMain.newKeys & A_BUTTON)
    {
		OptionMenu_ClearWindow(DESCRIPTION);
        gTasks[taskId].data[TD_TIMER] = 20;
		gTasks[taskId].func = Task_OptionMenuSave;
    }
	else if (gMain.newKeys & B_BUTTON)
    {
		OptionMenu_ClearWindow(DESCRIPTION);
        gTasks[taskId].data[TD_TIMER] = 20;
		gTasks[taskId].func = Task_OptionMenuCancel;
    }
    else if (gMain.newKeys & DPAD_UP)
    {
        if (gTasks[taskId].data[TD_MENUSELECTION] > 0)
            gTasks[taskId].data[TD_MENUSELECTION]--;
        else
            gTasks[taskId].data[TD_MENUSELECTION] = 5;
		LZ77UnCompVram(sOptionMenuItems[gTasks[taskId].data[TD_MENUSELECTION]], (void *)(VRAM + 0x3800));
		OptionMenu_ClearWindow(DESCRIPTION);
		SetDescription(gTasks[taskId].data[TD_MENUSELECTION]);
    }
    else if (gMain.newKeys & DPAD_DOWN)
    {
        if (gTasks[taskId].data[TD_MENUSELECTION] < 5)
            gTasks[taskId].data[TD_MENUSELECTION]++;
        else
            gTasks[taskId].data[TD_MENUSELECTION] = 0;
		LZ77UnCompVram(sOptionMenuItems[gTasks[taskId].data[TD_MENUSELECTION]], (void *)(VRAM + 0x3800));
		OptionMenu_ClearWindow(DESCRIPTION);
		SetDescription(gTasks[taskId].data[TD_MENUSELECTION]);
    }
    else
    {
        u8 previousOption;

        switch (gTasks[taskId].data[TD_MENUSELECTION])
        {
        case MENUITEM_TEXTSPEED:
			previousOption = gTasks[taskId].data[TD_TEXTSPEED];
            gTasks[taskId].data[TD_TEXTSPEED] = TextSpeed_ProcessInput(gTasks[taskId].data[TD_TEXTSPEED]);

            if (previousOption != gTasks[taskId].data[TD_TEXTSPEED])
                TextSpeed_DrawChoices(gTasks[taskId].data[TD_TEXTSPEED]);
            break;
        case MENUITEM_BATTLESCENE:
            previousOption = gTasks[taskId].data[TD_BATTLESCENE];
            gTasks[taskId].data[TD_BATTLESCENE] = BattleScene_ProcessInput(gTasks[taskId].data[TD_BATTLESCENE]);

            if (previousOption != gTasks[taskId].data[TD_BATTLESCENE])
                BattleScene_DrawChoices(gTasks[taskId].data[TD_BATTLESCENE]);
			break;
        case MENUITEM_BATTLESTYLE:
            previousOption = gTasks[taskId].data[TD_BATTLESTYLE];
            gTasks[taskId].data[TD_BATTLESTYLE] = BattleStyle_ProcessInput(gTasks[taskId].data[TD_BATTLESTYLE]);

            if (previousOption != gTasks[taskId].data[TD_BATTLESTYLE])
                BattleStyle_DrawChoices(gTasks[taskId].data[TD_BATTLESTYLE]);
            break;
        case MENUITEM_SOUND:
            previousOption = gTasks[taskId].data[TD_SOUND];
            gTasks[taskId].data[TD_SOUND] = Sound_ProcessInput(gTasks[taskId].data[TD_SOUND]);

            if (previousOption != gTasks[taskId].data[TD_SOUND])
                Sound_DrawChoices(gTasks[taskId].data[TD_SOUND]);
            break;
        case MENUITEM_BUTTONMODE:
            previousOption = gTasks[taskId].data[TD_BUTTONMODE];
            gTasks[taskId].data[TD_BUTTONMODE] = ButtonMode_ProcessInput(gTasks[taskId].data[TD_BUTTONMODE]);

            if (previousOption != gTasks[taskId].data[TD_BUTTONMODE])
                ButtonMode_DrawChoices(gTasks[taskId].data[TD_BUTTONMODE]);
			break;
        case MENUITEM_FRAMETYPE:
            previousOption = gTasks[taskId].data[TD_FRAMETYPE];
            gTasks[taskId].data[TD_FRAMETYPE] = FrameType_ProcessInput(gTasks[taskId].data[TD_FRAMETYPE]);

            if (previousOption != gTasks[taskId].data[TD_FRAMETYPE])
                FrameType_DrawChoices(gTasks[taskId].data[TD_FRAMETYPE]);
            break;
        default:
            return;
        }

        if (sArrowPressed)
        {
            sArrowPressed = FALSE;
            CopyWindowToVram(0, 2);
        }
    }
}

static void OptionMenu_ClearWindow(u8 option)
{
	if (option == DESCRIPTION)
	{	
		FillWindowPixelRect(0, 0, 8, 120, 224, 32);
	}
	else //(option == FRAME)
	{	
		FillWindowPixelRect(0, 0, 142, 96, 16, 16);
	}
	CopyWindowToVram(0, 2);
}

static void Task_OptionMenuSave(u8 taskId)
{
	SetDescription(6);
	if (gTasks[taskId].data[TD_TIMER])
    {
        gTasks[taskId].data[TD_TIMER]--;
    }
    else
	{
		gSaveBlock2Ptr->optionsTextSpeed = gTasks[taskId].data[TD_TEXTSPEED];
		gSaveBlock2Ptr->optionsBattleSceneOff = gTasks[taskId].data[TD_BATTLESCENE];
		gSaveBlock2Ptr->optionsBattleStyle = gTasks[taskId].data[TD_BATTLESTYLE];
		gSaveBlock2Ptr->optionsSound = gTasks[taskId].data[TD_SOUND];
		gSaveBlock2Ptr->optionsButtonMode = gTasks[taskId].data[TD_BUTTONMODE];
		gSaveBlock2Ptr->optionsWindowFrameType = gTasks[taskId].data[TD_FRAMETYPE];

		BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, 0);
		gTasks[taskId].func = Task_OptionMenuFadeOut;
	}
}

static void Task_OptionMenuCancel(u8 taskId)
{
	SetDescription(7);
	if (gTasks[taskId].data[TD_TIMER])
    {
        gTasks[taskId].data[TD_TIMER]--;
    }
    else
	{
		BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, 0);
		gTasks[taskId].func = Task_OptionMenuFadeOut;
	}
}

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[160];
    u16 i;

    for (i = 0; *text != EOS && i <= 160; i++)
        dst[i] = *(text++);
	
	dst[2] = style;
    dst[i] = EOS;
    AddTextPrinterParameterized(0, 1, dst, x, y + 1, TEXT_SPEED_FF, NULL);
	CopyWindowToVram(0, 2);
}

static void SetDescription(u8 selection)
{
	ShowDescription(sOptionMenuDescriptions[selection]);
}

static void ShowDescription(const u8 *text)
{
	u8 dst[160];
    u16 i;

    for (i = 0; *text != EOS && i <= 160; i++)
        dst[i] = *(text++);
	
	dst[2] = 5;
    dst[i] = EOS;
	AddTextPrinterParameterized4(0, 1, 12, 120, 0, 0, 0, 0, dst);
}

static u8 TextSpeed_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection < 2)
            selection++;
        else
            selection = 0;

        sArrowPressed = TRUE;
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection != 0)
            selection--;
        else
            selection = 2;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void TextSpeed_DrawChoices(u8 selection)
{
    u8 styles[3];

    styles[0] = 5;
    styles[1] = 5;
    styles[2] = 5;
    styles[selection] = 1;

    DrawOptionMenuChoice(localText_TextSpeedSlow, 112, 16, styles[0]);
    DrawOptionMenuChoice(localText_TextSpeedMid,  148, 16, styles[1]);
    DrawOptionMenuChoice(localText_TextSpeedFast, 184, 16, styles[2]);
}

static u8 BattleScene_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void BattleScene_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 5;
    styles[1] = 5;
    styles[selection] = 1;

    DrawOptionMenuChoice(localText_BattleSceneOn,  112, 32, styles[0]);
    DrawOptionMenuChoice(localText_BattleSceneOff, 162, 32, styles[1]);
}

static u8 BattleStyle_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void BattleStyle_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 5;
    styles[1] = 5;
    styles[selection] = 1;

    DrawOptionMenuChoice(localText_BattleStyleShift, 112, 48, styles[0]);
    DrawOptionMenuChoice(localText_BattleStyleSet,   162, 48, styles[1]);
}

static u8 Sound_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
        sArrowPressed = TRUE;
    }

    return selection;
}

static void Sound_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 5;
    styles[1] = 5;
    styles[selection] = 1;

    DrawOptionMenuChoice(localText_SoundMono,   112, 64, styles[0]);
    DrawOptionMenuChoice(localText_SoundStereo, 162, 64, styles[1]);
}

static u8 FrameType_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection < WINDOW_FRAMES_COUNT_FR - 1)
            selection++;
        else
            selection = 0;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0xE0, 0x20);
        sArrowPressed = TRUE;
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection != 0)
            selection--;
        else
            selection = WINDOW_FRAMES_COUNT_FR - 1;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, 0xE0, 0x20);
        sArrowPressed = TRUE;
    }
    return selection;
}

static void FrameType_DrawChoices(u8 selection)
{
    u8 text[16];
    u8 n = selection + 1;
    u16 i;

    for (i = 0; localText_FrameTypeNumber[i] != EOS && i <= 5; i++)
        text[i] = localText_FrameTypeNumber[i];

    // Convert a number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = CHAR_SPACE;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = 0x77;
        i++;
    }

    text[i] = EOS;
	
	OptionMenu_ClearWindow(FRAME);
	DrawOptionMenuChoice(localText_FrameType, 112, 96, 1);
    DrawOptionMenuChoice(text, 142, 96, 1);
}

static u8 ButtonMode_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection <= 1)
            selection++;
        else
            selection = 0;

        sArrowPressed = TRUE;
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection != 0)
            selection--;
        else
            selection = 2;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void ButtonMode_DrawChoices(u8 selection)
{
    u8 styles[3];

    styles[0] = 5;
    styles[1] = 5;
    styles[2] = 5;
    styles[selection] = 1;

    DrawOptionMenuChoice(localText_ButtonTypeNormal,   112, 80, styles[0]);
    DrawOptionMenuChoice(localText_ButtonTypeLR,       162, 80, styles[1]);
    DrawOptionMenuChoice(localText_ButtonTypeLEqualsA, 188, 80, styles[2]);
}

static void DrawOptionMenuTexts(void)
{
    u8 i;

    FillWindowPixelBuffer(0, PIXEL_FILL(0));
	AddTextPrinterParameterized(0, 1, localText_Option, 8, 1, TEXT_SPEED_FF, NULL);
	AddTextPrinterParameterized(0, 1, localText_Instructions, 140, 0 , TEXT_SPEED_FF, NULL);
	SetDescription(0);
	for (i = 0; i < MENUITEM_COUNT; i++)
    {
        AddTextPrinterParameterized(0, 1, sOptionMenuItemsNames[i], 8, (i * 16) + 17, TEXT_SPEED_FF, NULL);
    }
    CopyWindowToVram(0, 2);
}

static void DrawFrame(void)
{   //                     bg, tileNum,  x,    y,  width, height,  pal
    FillBgTilemapBufferRect(2,   0x1A2,  0,   14,      1,      1,   14);
    FillBgTilemapBufferRect(2, 	 0x1A3,  1,   14,     28,      1,   14);
    FillBgTilemapBufferRect(2,   0x1A4, 29,   14,      1,      1,   14);
    FillBgTilemapBufferRect(2,   0x1A5,  0,   15,      1,      4,   14);
    FillBgTilemapBufferRect(2,   0x1A6,  1,   15,     28,      4,   14);
    FillBgTilemapBufferRect(2,   0x1A7, 29,   15,      1,      4,   14);
    FillBgTilemapBufferRect(2,   0x1A8,  0,   19,      1,      1,   14);
    FillBgTilemapBufferRect(2,   0x1A9,  1,   19,     28,      1,   14);
    FillBgTilemapBufferRect(2,   0x1AA, 29,   19,      1,      1,   14);

    CopyBgTilemapBufferToVram(2);
}

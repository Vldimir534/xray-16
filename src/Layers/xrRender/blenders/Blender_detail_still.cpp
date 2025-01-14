#include "stdafx.h"
#pragma hdrstop

#include "Blender_detail_still.h"

CBlender_Detail_Still::CBlender_Detail_Still()
{
    description.CLS = B_DETAIL;
    description.version = 0;
}

void CBlender_Detail_Still::Save(IWriter& fs)
{
    IBlender::Save(fs);
    xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
}

void CBlender_Detail_Still::Load(IReader& fs, u16 version)
{
    IBlender::Load(fs, version);
    xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
}

LPCSTR CBlender_Detail_Still::getComment()
{
    return "LEVEL: detail objects";
}

void CBlender_Detail_Still::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    if (C.bFFP)
        CompileFFP(C);
    else
        CompileProgrammable(C);
}

void CBlender_Detail_Still::CompileFFP(CBlender_Compile& C) const
{
    if (C.bEditor)
    {
        C.PassBegin();
        {
            C.PassSET_ZB(TRUE, TRUE);
            if (oBlend.value)
                C.PassSET_Blend_BLEND(TRUE, 200);
            else
                C.PassSET_Blend_SET(TRUE, 200);
            C.PassSET_LightFog(TRUE, TRUE);

            // Stage1 - Base texture
            C.StageBegin();
            C.StageSET_Color(D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE);
            C.StageSET_Alpha(D3DTA_TEXTURE, D3DTOP_MODULATE, D3DTA_DIFFUSE);
            C.StageSET_TMC(oT_Name, "$null", "$null", 0);
            C.StageEnd();
        }
        C.PassEnd();
    }
    else
    {
        switch (C.iElement)
        {
        case SE_R1_NORMAL_HQ:
        {
            C.PassBegin();
            {
                C.PassSET_ZB(TRUE, TRUE);
                if (oBlend.value)
                    C.PassSET_Blend_BLEND(TRUE, 200);
                else
                    C.PassSET_Blend_SET(TRUE, 200);
                C.PassSET_LightFog(FALSE, FALSE);

                switch (C.iElement)
                {
                case 0: C.PassSET_Shaders("detail_wave", "null"); break;
                case 1: C.PassSET_Shaders("detail_still", "null"); break;
                }

                // Stage1 - Base texture
                C.StageBegin();
                C.StageSET_Color(D3DTA_TEXTURE, D3DTOP_MODULATE2X, D3DTA_DIFFUSE);
                C.StageSET_Alpha(D3DTA_TEXTURE, D3DTOP_MODULATE2X, D3DTA_DIFFUSE);
                C.StageSET_TMC(oT_Name, "$null", "$null", 0);
                C.StageEnd();
            }
            C.PassEnd();
            break;
        }
        case SE_R1_LMODELS:
        {
            C.PassBegin();
            {
                C.PassSET_ZB(TRUE, TRUE);
                if (oBlend.value)
                    C.PassSET_Blend_BLEND(TRUE, 200);
                else
                    C.PassSET_Blend_SET(TRUE, 200);
                C.PassSET_LightFog(FALSE, FALSE);
                C.PassSET_Shaders("detail_still", "null");

                // Stage1 - Base texture
                C.StageBegin();
                C.StageSET_Color(D3DTA_TEXTURE, D3DTOP_SELECTARG2, D3DTA_DIFFUSE);
                C.StageSET_Alpha(D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE);
                C.StageSET_TMC(oT_Name, "$null", "$null", 0);
                C.StageEnd();
            }
            C.PassEnd();
        }

        default:
            break;
        } // switch (C.iElement)
    }
}

void CBlender_Detail_Still::CompileProgrammable(CBlender_Compile& C) const
{
    switch (C.iElement)
    {
    case SE_R1_NORMAL_HQ:
        C.r_Pass("detail_wave", "detail", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO,
            oBlend.value ? TRUE : FALSE, oBlend.value ? 200 : 0);
        C.r_Sampler("s_base", C.L_textures[0]);
        C.r_End();
        break;

    case SE_R1_NORMAL_LQ:
        C.r_Pass("detail_still", "detail", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO,
            oBlend.value ? TRUE : FALSE, oBlend.value ? 200 : 0);
        C.r_Sampler("s_base", C.L_textures[0]);
        C.r_End();
        break;

    default:
        break;
    } // switch (C.iElement)
}

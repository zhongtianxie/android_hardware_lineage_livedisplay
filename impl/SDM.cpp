/*
** Copyright 2016, The CyanogenMod Project
**           2017, The LineageOS Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cutils/properties.h>

//#define LOG_NDEBUG 0

#define LOG_TAG "LiveDisplay-SDM"
#include <utils/Log.h>

#include "SDM.h"
#include "Utils.h"

namespace android {

status_t SDM::loadVendorLibrary() {
    if (mLibHandle != NULL) {
        return OK;
    }

    mLibHandle = dlopen(SDM_DISP_LIB, RTLD_NOW);
    if (mLibHandle == NULL) {
        ALOGE("DLOPEN failed for %s (%s)", SDM_DISP_LIB, dlerror());
        ALOGE("Attempting to DLOPEN legacy lib %s", SDM_DISP_LEGACY_LIB);
        mLibHandle = dlopen(SDM_DISP_LEGACY_LIB, RTLD_NOW);
        if (mLibHandle == NULL) {
            ALOGE("DLOPEN failed for legacy lib %s (%s)", SDM_DISP_LEGACY_LIB, dlerror());
            return NO_INIT;
        } else {
            ALOGE("%s successfully DLOPENed. This is deprecated! Switch to new %s",
                   SDM_DISP_LEGACY_LIB, SDM_DISP_LIB);
        }
    }

    disp_api_init = (int32_t(*)(int64_t*, uint32_t))dlsym(mLibHandle, "disp_api_init");
    if (disp_api_init == NULL) {
        ALOGE("dlsym failed for disp_api_init");
        goto fail;
    }
    disp_api_deinit = (int32_t(*)(int64_t, uint32_t))dlsym(mLibHandle, "disp_api_deinit");
    if (disp_api_deinit == NULL) {
        ALOGE("dlsym failed for disp_api_deinit");
        goto fail;
    }
    disp_api_get_global_color_balance_range = (int32_t(*)(int64_t, uint32_t, void*))dlsym(
        mLibHandle, "disp_api_get_global_color_balance_range");
    if (disp_api_get_global_color_balance_range == NULL) {
        ALOGE("dlsym failed for disp_api_get_global_color_balance_range");
        goto fail;
    }
    disp_api_set_global_color_balance = (int32_t(*)(int64_t, uint32_t, int32_t, uint32_t))dlsym(
        mLibHandle, "disp_api_set_global_color_balance");
    if (disp_api_set_global_color_balance == NULL) {
        ALOGE("dlsym failed for disp_api_set_global_color_balance");
        goto fail;
    }
    disp_api_get_global_color_balance = (int32_t(*)(int64_t, uint32_t, int32_t*, uint32_t*))dlsym(
        mLibHandle, "disp_api_get_global_color_balance");
    if (disp_api_get_global_color_balance == NULL) {
        ALOGE("dlsym failed for disp_api_get_global_color_balance");
        goto fail;
    }
    disp_api_get_num_display_modes =
        (int32_t(*)(int64_t, uint32_t, int32_t, int32_t*, uint32_t*))dlsym(
            mLibHandle, "disp_api_get_num_display_modes");
    if (disp_api_get_num_display_modes == NULL) {
        ALOGE("dlsym failed for disp_api_get_num_display_modes");
        goto fail;
    }
    disp_api_get_display_modes =
        (int32_t(*)(int64_t, uint32_t, int32_t, void*, int32_t, uint32_t*))dlsym(
            mLibHandle, "disp_api_get_display_modes");
    if (disp_api_get_display_modes == NULL) {
        ALOGE("dlsym failed for disp_api_get_display_modes");
        goto fail;
    }
    disp_api_get_active_display_mode =
        (int32_t(*)(int64_t, uint32_t, int32_t*, uint32_t*, uint32_t*))dlsym(
            mLibHandle, "disp_api_get_active_display_mode");
    if (disp_api_get_active_display_mode == NULL) {
        ALOGE("dlsym failed for disp_api_get_active_display_mode");
        goto fail;
    }
    disp_api_set_active_display_mode = (int32_t(*)(int64_t, uint32_t, int32_t, uint32_t))dlsym(
        mLibHandle, "disp_api_set_active_display_mode");
    if (disp_api_set_active_display_mode == NULL) {
        ALOGE("dlsym failed for disp_api_set_active_display_mode");
        goto fail;
    }
    disp_api_set_default_display_mode = (int32_t(*)(int64_t, uint32_t, int32_t, uint32_t))dlsym(
        mLibHandle, "disp_api_set_default_display_mode");
    if (disp_api_set_default_display_mode == NULL) {
        ALOGE("dlsym failed for disp_api_set_default_display_mode");
        goto fail;
    }
    disp_api_get_default_display_mode = (int32_t(*)(int64_t, uint32_t, int32_t*, uint32_t*))dlsym(
        mLibHandle, "disp_api_get_default_display_mode");
    if (disp_api_get_default_display_mode == NULL) {
        ALOGE("dlsym failed for disp_api_get_default_display_mode");
        goto fail;
    }
    disp_api_get_global_pa_range =
        (int32_t(*)(int64_t, uint32_t, void*))dlsym(mLibHandle, "disp_api_get_global_pa_range");
    if (disp_api_get_global_pa_range == NULL) {
        ALOGE("dlsym failed for disp_api_get_global_pa_range");
        goto fail;
    }
    disp_api_get_global_pa_config = (int32_t(*)(int64_t, uint32_t, uint32_t*, void*))dlsym(
        mLibHandle, "disp_api_get_global_pa_config");
    if (disp_api_get_global_pa_config == NULL) {
        ALOGE("dlsym failed for disp_api_get_global_pa_config");
        goto fail;
    }
    disp_api_set_global_pa_config = (int32_t(*)(int64_t, uint32_t, uint32_t, void*))dlsym(
        mLibHandle, "disp_api_set_global_pa_config");
    if (disp_api_set_global_pa_config == NULL) {
        ALOGE("dlsym failed for disp_api_set_global_pa_config");
        goto fail;
    }
    disp_api_get_feature_version = (int32_t(*)(int64_t, uint32_t, void*, uint32_t*))dlsym(
        mLibHandle, "disp_api_get_feature_version");
    if (disp_api_get_feature_version == NULL) {
        ALOGE("dlsym failed for disp_api_get_feature_version");
        goto fail;
    }

    return OK;

fail:
    ALOGE("Failed to link vendor library: %s", dlerror());
    dlclose(mLibHandle);
    mLibHandle = NULL;
    return NO_INIT;
}

status_t SDM::initialize() {
    status_t rc = loadVendorLibrary();
    if (rc != OK) {
        return rc;
    }

    rc = disp_api_init(&mHandle, 0);
    if (rc != OK) {
        return rc;
    }

    mActiveModeId = -1;

    if (hasFeature(Feature::DISPLAY_MODES)) {
        rc = saveInitialDisplayMode();
        if (rc != OK) {
            ALOGE("Failed to save initial display mode! err=%d", rc);
            return rc;
        }
        sp<DisplayMode> defMode = getDefaultDisplayMode();
        if (defMode != nullptr) {
            setDisplayMode(defMode->id, false);
        }
    }
    return OK;
}

SDM::~SDM() {
    if (mLibHandle != NULL) {
        dlclose(mLibHandle);
    }
}

status_t SDM::deinitialize() {
    if (mLibHandle != NULL) {
        disp_api_deinit(mHandle, 0);
        mHandle = -1;
    }
    return OK;
}

status_t SDM::setAdaptiveBacklightEnabled(bool enabled) {
    status_t rc = NO_INIT;
    if (enabled == mCachedFOSSStatus) {
        return OK;
    }
    char* buf = new char[DPPS_BUF_SIZE];
    sprintf(buf, "%s", enabled ? FOSS_ON : FOSS_OFF);
    if (Utils::sendDPPSCommand(buf, DPPS_BUF_SIZE) == OK) {
        if (strncmp(buf, "Success", 7) == 0) {
            rc = OK;
            mCachedFOSSStatus = enabled;
        }
    }
    delete[] buf;
    return rc;
}

bool SDM::isAdaptiveBacklightEnabled() {
    return mCachedFOSSStatus;
}

status_t SDM::getColorBalanceRange(Range& range) {
    status_t rc = disp_api_get_global_color_balance_range(mHandle, 0, &range);
    ALOGV("getColorBalanceRange: min=%d max=%d step=%d", range.min, range.max, range.step);
    return rc;
}

status_t SDM::setColorBalance(int32_t balance) {
    return disp_api_set_global_color_balance(mHandle, 0, balance, 0);
}

int32_t SDM::getColorBalance() {
    int32_t value = -1;
    uint32_t flags = 0;
    if (disp_api_get_global_color_balance(mHandle, 0, &value, &flags) != 0) {
        value = 0;
    }
    return value;
}

uint32_t SDM::getNumSDMDisplayModes() {
    uint32_t flags = 0;
    int32_t count = 0;
    if (disp_api_get_num_display_modes(mHandle, 0, 0, &count, &flags)) {
        count = 0;
    }
    return count;
}

uint32_t SDM::getNumDisplayModes() {
    int32_t count = getNumSDMDisplayModes();
    if (getLocalSRGBMode() != nullptr) {
        count++;
    }
    if (getLocalDCIP3Mode() != nullptr) {
        count++;
    }
    return count;
}

status_t SDM::getDisplayModes(List<sp<DisplayMode>>& profiles) {
    status_t rc = OK;
    uint32_t flags = 0, i = 0;

    uint32_t count = getNumDisplayModes();
    if (!count) return rc;

    sp<DisplayMode> srgb = getLocalSRGBMode();
    sp<DisplayMode> dci_p3 = getLocalDCIP3Mode();

    uint32_t sdm_count = getNumSDMDisplayModes();

    struct sdm_mode {
        int32_t id;
        int32_t type;
        int32_t len;
        char* name;
    };

    sdm_mode* tmp = new sdm_mode[sdm_count];
    memset(tmp, 0, sizeof(sdm_mode) * sdm_count);
    for (i = 0; i < sdm_count; i++) {
        tmp[i].id = -1;
        tmp[i].name = new char[128];
        tmp[i].len = 128;
    }

    rc = disp_api_get_display_modes(mHandle, 0, 0, tmp, sdm_count, &flags);
    if (rc == 0) {
        for (i = 0; i < sdm_count; i++) {
            const sp<DisplayMode> m = new DisplayMode(tmp[i].id, tmp[i].name, tmp[i].len);
            m->privFlags = PRIV_MODE_FLAG_SDM;
            profiles.push_back(m);
            delete tmp[i].name;
        }
    }
    delete[] tmp;

    if (sdm_count == 0) {
        const sp<DisplayMode> m = new DisplayMode(STANDARD_NODE_ID, "standard", 8);
        m->privFlags = PRIV_MODE_FLAG_SYSFS;
        m->privData.setTo("");
        profiles.push_back(m);
    }

    if (srgb != nullptr) {
        profiles.push_back(srgb);
    }
    if (dci_p3 != nullptr) {
        profiles.push_back(dci_p3);
    }

    return rc;
}

status_t SDM::setDisplayMode(int32_t modeID, bool makeDefault) {
    status_t rc = OK;

    if (modeID == mActiveModeId) {
        return OK;
    }

    sp<DisplayMode> mode = getDisplayModeById(modeID);
    if (mode == nullptr) {
        return BAD_VALUE;
    }

    ALOGV("setDisplayMode: current mode=%d", mActiveModeId);

    if (mActiveModeId >= 0) {
        sp<DisplayMode> oldMode = getCurrentDisplayMode();
        ALOGV("setDisplayMode: oldMode=%d flags=%d", oldMode->id, oldMode->privFlags);
        if (oldMode->privFlags == PRIV_MODE_FLAG_SYSFS ||
                mode->privFlags == PRIV_MODE_FLAG_SYSFS) {
            ALOGV("disabling old mode");
            rc = setModeState(oldMode, false);
            if (rc != OK) {
                ALOGE("Failed to disable previous mode! err=%d", rc);
                return rc;
            }
        }
    }

    rc = setModeState(mode, true);
    if (rc == OK) {
        mActiveModeId = mode->id;
        if (makeDefault) {
            rc = Utils::writeLocalModeId(mode->id);
            if (rc != OK) {
                ALOGE("failed to save mode! %d", rc);
                return rc;
            }
            if (mode->privFlags == PRIV_MODE_FLAG_SDM) {
                rc = disp_api_set_default_display_mode(mHandle, 0, mode->id, 0);
                if (rc != OK) {
                    ALOGE("failed to save mode! %d", rc);
                    return rc;
                }
            }
        }
        HSIC tmp;
        rc = getPictureAdjustment(tmp);
        if (rc != OK) {
            ALOGE("failed to retrieve picture adjustment after mode setting!");
        } else {
            ALOGV("new default PA: %d %f %f %f %f", tmp.hue, tmp.saturation,
                    tmp.intensity, tmp.contrast, tmp.saturationThreshold);
            mDefaultPictureAdjustment.setTo(tmp);
        }
    } else {
        ALOGE("Failed to setModeState! err=%d", rc);
        return rc;
    }

    ALOGV("setDisplayMode: %d default: %d flags: %d", modeID, makeDefault, mode->privFlags);
    return OK;
}

sp<DisplayMode> SDM::getDisplayModeById(int32_t id) {
    List<sp<DisplayMode>> profiles;
    status_t rc = getDisplayModes(profiles);
    if (rc == OK) {
        for (List<sp<DisplayMode>>::iterator it = profiles.begin(); it != profiles.end(); ++it) {
            const sp<DisplayMode> mode = *it;
            if (id == mode->id) {
                return mode;
            }
        }
    }

    return nullptr;
}

sp<DisplayMode> SDM::getCurrentDisplayMode() {
    return getDisplayModeById(mActiveModeId);
}

sp<DisplayMode> SDM::getDefaultDisplayMode() {
    int32_t id = 0;
    if (Utils::readLocalModeId(&id) == OK && id >= 0) {
        return getDisplayModeById(id);
    }
    if (Utils::readInitialModeId(&id) == OK && id >= 0) {
        return getDisplayModeById(id);
    }
    return nullptr;
}

status_t SDM::setModeState(sp<DisplayMode> mode, bool state) {
    int32_t id = 0;

    if (mode->privFlags == PRIV_MODE_FLAG_SYSFS) {
        if (mode->id != STANDARD_NODE_ID) {
           ALOGV("sysfs node: %s state=%d", mode->privData.string(), state);
           return Utils::writeInt(mode->privData.string(), state ? 1 : 0);
        } else {
           // NOOP
           return OK;
        }
    } else if (mode->privFlags == PRIV_MODE_FLAG_SDM) {
        if (state) {
            return disp_api_set_active_display_mode(mHandle, 0, mode->id, 0);
        } else {
            if (Utils::readInitialModeId(&id) == OK) {
                ALOGV("set sdm mode to default: id=%d", id);
                return disp_api_set_active_display_mode(mHandle, 0, id, 0);
            }
        }
    }
    return BAD_VALUE;
}

sp<DisplayMode> SDM::getLocalSRGBMode() {
    char path[PATH_MAX];
    sprintf(path, "%s", SRGB_NODE);

    if (access(path, W_OK) != 0) {
        return nullptr;
    }
    sp<DisplayMode> m = new DisplayMode(SRGB_NODE_ID, "srgb", 4);
    m->privFlags = PRIV_MODE_FLAG_SYSFS;
    m->privData.setTo(path);
    return m;
}

sp<DisplayMode> SDM::getLocalDCIP3Mode() {
    char path[PATH_MAX];
    sprintf(path, "%s", DCI_P3_NODE);

    if (access(path, W_OK) != 0) {
        return nullptr;
    }
    sp<DisplayMode> m = new DisplayMode(DCI_P3_NODE_ID, "dci_p3", 6);
    m->privFlags = PRIV_MODE_FLAG_SYSFS;
    m->privData.setTo(path);
    return m;
}

status_t SDM::getPictureAdjustmentRanges(HSICRanges& ranges) {
    hsic_ranges r;
    memset(&r, 0, sizeof(struct hsic_ranges));

    status_t rc = disp_api_get_global_pa_range(mHandle, 0, &r);
    if (rc == OK) {
        ranges.hue.min = r.hue.min;
        ranges.hue.max = r.hue.max;
        ranges.hue.step - r.hue.step;
        ranges.saturation.min = r.saturation.min;
        ranges.saturation.max = r.saturation.max;
        ranges.saturation.step = r.saturation.step;
        ranges.intensity.min = r.intensity.min;
        ranges.intensity.max = r.intensity.max;
        ranges.intensity.step = r.intensity.step;
        ranges.contrast.min = r.contrast.min;
        ranges.contrast.max = r.contrast.max;
        ranges.contrast.step = r.contrast.step;
        ranges.saturationThreshold.min = r.saturationThreshold.min;
        ranges.saturationThreshold.max = r.saturationThreshold.max;
        ranges.saturationThreshold.step = r.saturationThreshold.step;
    }
    return rc;
}

status_t SDM::getPictureAdjustment(HSIC& hsic) {
    uint32_t enable = 0;
    hsic_config config;
    memset(&config, 0, sizeof(struct hsic_config));

    status_t rc = disp_api_get_global_pa_config(mHandle, 0, &enable, &config);
    if (rc == OK) {
        hsic.hue = config.data.hue;
        hsic.saturation = config.data.saturation;
        hsic.intensity = config.data.intensity;
        hsic.contrast = config.data.contrast;
        hsic.saturationThreshold = config.data.saturationThreshold;
    }
    return rc;
}

status_t SDM::getDefaultPictureAdjustment(HSIC& hsic) {
    hsic.setTo(mDefaultPictureAdjustment);
    return OK;
}

status_t SDM::setPictureAdjustment(HSIC hsic) {
    hsic_config config;
    memset(&config, 0, sizeof(struct hsic_config));
    config.data.hue = hsic.hue;
    config.data.saturation = hsic.saturation;
    config.data.intensity = hsic.intensity;
    config.data.contrast = hsic.contrast;
    config.data.saturationThreshold = hsic.saturationThreshold;

    return disp_api_set_global_pa_config(mHandle, 0, 1, &config);
}

bool SDM::hasFeature(Feature feature) {
    uint32_t id = 0, flags = 0;
    struct version {
        uint8_t x, y;
        uint16_t z;
    };
    version v;

    switch (feature) {
        case Feature::DISPLAY_MODES:
            id = 4;
            break;
        case Feature::COLOR_TEMPERATURE:
            id = 3;
            break;
        case Feature::PICTURE_ADJUSTMENT:
            id = 1;
        case Feature::ADAPTIVE_BACKLIGHT:
            if (property_get_int32("ro.qualcomm.foss", 0) > 0) {
                return true;
            }
            break;
        default:
            return false;
    }

    if (disp_api_get_feature_version(mHandle, id, &v, &flags) == 0) {
        if (v.x > 0 || v.y > 0 || v.z > 0) {

            // Color balance depends on calibration data in SDM
            if (feature == Feature::DISPLAY_MODES ||
                    feature == Feature::COLOR_TEMPERATURE) {
                if (getNumDisplayModes() > 0) {
                    // make sure the range isn't zero
                    if (feature == Feature::COLOR_TEMPERATURE) {
                        Range r;
                        if (getColorBalanceRange(r) == OK && r.isNonZero()) {
                            return true;
                        }
                        return false;
                    }
                    return true;
				}
            } else if (feature == Feature::PICTURE_ADJUSTMENT) {
                HSICRanges r;
                if (getPictureAdjustmentRanges(r) == OK && r.isValid()) {
                    return true;
                }
            }
        }
    }
    return false;
}

status_t SDM::saveInitialDisplayMode() {
    int32_t id = 0;
    uint32_t flags = 0;
    if (Utils::readInitialModeId(&id) != OK || id < 0) {
        if (disp_api_get_default_display_mode(mHandle, 0, &id, &flags) == OK && id >= 0) {
            return Utils::writeInitialModeId(id);
        } else {
            return Utils::writeInitialModeId(id = 0);
        }
    }
    return OK;
}

};

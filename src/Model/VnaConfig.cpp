// Config (Model). Stores the state of client-server parameters.
// Communicates with the Presenter on the main thread.

#include "Model/VnaConfig.h"


VnaConfigModel::VnaConfigModel(QObject* parent)
    : QObject(parent){}

// We get data through the getter (copy of the structure).
VnaConfig VnaConfigModel::getConfig() const
{
    return m_cfg;
}

// Save data setter.
void VnaConfigModel::setConfig(const VnaConfig& cfg)
{
    if (m_cfg.startFreq != cfg.startFreq) {
        m_cfg.startFreq = cfg.startFreq;
    }
    if (m_cfg.stopFreq != cfg.stopFreq) {
        m_cfg.stopFreq = cfg.stopFreq;
    }
    if (m_cfg.points != cfg.points) {
        m_cfg.points = cfg.points;
    }
    if (m_cfg.power != cfg.power) {
        m_cfg.power = cfg.power;
    }
    if (m_cfg.ifBw != cfg.ifBw) {
        m_cfg.ifBw = cfg.ifBw;
    }
}

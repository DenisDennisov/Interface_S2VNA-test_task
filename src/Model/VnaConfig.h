// Config (Model) module.
// Stores all parameters that can be read / written SCPI commands.

#ifndef VNACONFIG_H
#define VNACONFIG_H

#include "Interfaces/IVnaConfig.h"
#include <QObject>


class VnaConfigModel : public QObject, public IConfigModel
{
    Q_OBJECT
public:
    explicit VnaConfigModel(QObject *parent = nullptr);
    ~VnaConfigModel() override = default;

    // ======== Getter data ========
    VnaConfig getConfig() const override;

    // ======== Setter data ========
    void setConfig(const VnaConfig&) override;

private:
    VnaConfig m_cfg;
};

#endif // VNACONFIG_H

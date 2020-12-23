/*
 *  Copyright (C) 2014 Benjamin W. (bitbatzen@gmail.com)
 *
 *  This file is part of Netvisix.
 *
 *  Netvisix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Netvisix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Netvisix.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "VisibleHost.h"
#include "NetView.h"
#include "Net/NetEventManager.h"
#include "Net/NetEvent.h"
#include "Net/Host.h"
#include "GUI/VisiblePacket.h"

#include <QWidget>


namespace Netvisix {

    const Qt::BrushStyle VisibleHost::MULTICAST_BRUSH_STYLE = Qt::BrushStyle::Dense2Pattern;

    VisibleHost::VisibleHost(NetView* netView, Host* host) : VisibleBase(netView) {
        this->host = host;

        color = QColor(160, 160, 160, 255);
        brush = QBrush(color, Qt::BrushStyle::SolidPattern);

        colorAliveEffect = QColor(110, 110, 130, 255);
        brushAliveEffect = QBrush(colorAliveEffect, Qt::BrushStyle::SolidPattern);
        aliveEffectTimer = 0;
        aliveEffectActive = true;

        isMarked = false;
        brushMarked = QBrush(Qt::green, Qt::BrushStyle::SolidPattern);

        colorLocalInterfaceMarker = QColor(190, 190, 190, 255);

        visibleTimer    = 0;
        isVisible       = true;

        speed = 1.5f;

        setRadius(12);

        setPosition(netView->getFreeHostPosition(this));
        targetPos = getPosition();

        // multicast effect
        mcEffectDataList = new std::vector<MulticastEffectData*>();
        for (int i = 0; i < 10; i++) {
            MulticastEffectData* mced = new MulticastEffectData();
            mcEffectDataList->push_back(mced);
        }
    }

    VisibleHost::~VisibleHost() {
        for (unsigned int i = 0; i < mcEffectDataList->size(); i++) {
            delete mcEffectDataList->at(i);
        }
        delete mcEffectDataList;
    }

    void VisibleHost::showMulticastEffect(NetEvent* netEvent) {
        // get next free mced
        MulticastEffectData* mced = nullptr;
        for (unsigned int i = 0; i < mcEffectDataList->size(); i++) {
            MulticastEffectData* m = mcEffectDataList->at(i);
            if (m->isActive == false) {
                mced = m;
            }
        }

        if (mced == nullptr) {
            mced = mcEffectDataList->at(0);
        }

        mced->isActive = true;
        mced->currentRadius = getRadius();

        mced->color = VisiblePacket::getPacketColor(netEvent->getTopLevelProtocol());

        if (netEvent->isIPv6()) {
             mced->brush = QBrush(mced->color, MULTICAST_BRUSH_STYLE);
        }
        else {
             mced->brush = QBrush(mced->color, Qt::BrushStyle::SolidPattern);
        }

        delete netEvent;
    }

    void VisibleHost::showHostAliveEffect() {
        if (! getIsVisible()) {
            setPosition(targetPos);
        }
        visibleTimer    = 0;
        isVisible       = true;

        aliveEffectTimer = 0;
        brushAliveEffect.setColor(colorAliveEffect);
        aliveEffectActive = true;
    }

    void VisibleHost::onVisibleUpdate(quint64 dt) {
        if (! getIsVisible()) {
            return;
        }

        if (host->getNetArea() != NetArea::LOCAL_INTERFACE) {
            visibleTimer += dt;
            if (visibleTimer >= VISIBLE_DURATION) {
                isVisible = false;
                return;
            }
        }

        VisibleBase::onVisibleUpdate(dt);

        // multicast effect
        const float radiusMax = getRadius() * 3.5f;
        const float mcEffectSpeed = 0.02f;
        for (unsigned int i = 0; i < mcEffectDataList->size(); i++) {
            MulticastEffectData* m = mcEffectDataList->at(i);
            if (m->isActive) {
                m->currentRadius += mcEffectSpeed * dt;
                m->currentRadius = std::min(m->currentRadius, radiusMax);
                float percentage = 1.0f - (m->currentRadius - getRadius()) / (radiusMax - getRadius());
                percentage = std::min(1.0f, percentage);
                percentage = std::max(0.0f, percentage);
                m->brush.setColor(QColor(m->color.red(), m->color.green(), m->color.blue(), m->color.alpha() * percentage));

                if (m->currentRadius >= radiusMax) {
                    m->isActive = false;
                }
            }
        }

        if (aliveEffectActive) {
            aliveEffectTimer += dt;
            if (aliveEffectTimer >= ALIVE_EFFECT_STAY_DURATION) {
                float percentage = 1.0f - (float) (aliveEffectTimer - ALIVE_EFFECT_STAY_DURATION) / ALIVE_EFFECT_FADE_OUT_DURATION;
                percentage = std::min(1.0f, percentage);
                percentage = std::max(0.0f, percentage);
                brushAliveEffect.setColor(QColor(colorAliveEffect.red(), colorAliveEffect.green(), colorAliveEffect.blue(), colorAliveEffect.alpha() * percentage));
                if (aliveEffectTimer >= ALIVE_EFFECT_STAY_DURATION + ALIVE_EFFECT_FADE_OUT_DURATION) {
                    aliveEffectActive = false;
                }
            }
        }
    }

    void VisibleHost::onVisiblePaint(QPainter& painter) {
        if (! getIsVisible()) {
            return;
        }

        painter.setRenderHint(QPainter::Antialiasing, true);

        // multicast effect
        for (unsigned int i = 0; i < mcEffectDataList->size(); i++) {
            MulticastEffectData* m = mcEffectDataList->at(i);
            if (m->isActive) {
                painter.setBrush(m->brush);
                painter.drawEllipse(getPosition(), m->currentRadius, m->currentRadius);
            }
        }

        VisibleBase::onVisiblePaint(painter);

        if (aliveEffectActive) {
            painter.setBrush(brushAliveEffect);
            painter.drawEllipse(getPosition(), getRadius(), getRadius());
        }

        if (host->getNetArea() == NetArea::LOCAL_INTERFACE) {
            painter.setBrush(QBrush(colorLocalInterfaceMarker, Qt::BrushStyle::SolidPattern));
            painter.drawEllipse(getPosition(), getRadius() * 0.5f, getRadius() * 0.5f);
        }
        else if (isMarked) {
            painter.setBrush(brushMarked);
            painter.drawEllipse(getPosition(), getRadius() * 0.5f, getRadius() * 0.5f);
        }

        painter.setRenderHint(QPainter::Antialiasing, false);
    }

} // namespace Netvisix

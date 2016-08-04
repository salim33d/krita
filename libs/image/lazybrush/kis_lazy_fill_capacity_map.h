/*
 *  Copyright (c) 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_LAZY_FILL_CAPACITY_MAP_H
#define __KIS_LAZY_FILL_CAPACITY_MAP_H

#include <KoColorSpace.h>

#include "kis_lazy_fill_graph.h"
#include "kis_paint_device.h"
#include "kis_types.h"
#include "kis_painter.h"
#include "kis_random_accessor_ng.h"
#include "kis_global.h"


class KisLazyFillCapacityMap
{
    typedef KisLazyFillCapacityMap type;
    typedef typename boost::graph_traits<KisLazyFillGraph>::vertex_descriptor VertexDescriptor;
    typedef typename boost::graph_traits<KisLazyFillGraph>::edge_descriptor EdgeDescriptor;

public:
    typedef EdgeDescriptor key_type;
    typedef float value_type;
    typedef const float& reference;
    typedef boost::readable_property_map_tag category;

    KisLazyFillCapacityMap(KisPaintDeviceSP mainImage,
                           KisPaintDeviceSP aLabelImage,
                           KisPaintDeviceSP bLabelImage)
        : m_mainImage(mainImage),
          m_aLabelImage(KisPainter::convertToAlphaAsAlpha(aLabelImage)),
          m_bLabelImage(KisPainter::convertToAlphaAsAlpha(bLabelImage)),
          m_mainRect(m_mainImage->exactBounds()),
          m_aLabelRect(m_aLabelImage->exactBounds()),
          m_bLabelRect(m_bLabelImage->exactBounds()),
          m_colorSpace(mainImage->colorSpace()),
          m_pixelSize(m_colorSpace->pixelSize()),
          m_graph(m_mainRect, m_aLabelRect, m_bLabelRect)
    {
        const QPoint pt = m_mainRect.topLeft();

        m_mainAccessor = m_mainImage->createRandomConstAccessorNG(pt.x(), pt.y());
        m_aAccessor = m_aLabelImage->createRandomConstAccessorNG(pt.x(), pt.y());
        m_bAccessor = m_bLabelImage->createRandomConstAccessorNG(pt.x(), pt.y());
        m_srcPixelBuf.resize(m_pixelSize);
    }

    int maxCapacity() const {
        const int k  = 2 * (m_mainRect.width() + m_mainRect.height());
        return k + 1;
    }

    friend value_type get(type &map,
                          const key_type &key)
        {
            VertexDescriptor src = source(key, map.m_graph);
            VertexDescriptor dst = target(key, map.m_graph);

            bool srcLabelA = src.type == VertexDescriptor::LABEL_A;
            bool srcLabelB = src.type == VertexDescriptor::LABEL_B;
            bool dstLabelA = dst.type == VertexDescriptor::LABEL_A;
            bool dstLabelB = dst.type == VertexDescriptor::LABEL_B;

            if (srcLabelA || srcLabelB) {
                std::swap(src, dst);
                std::swap(srcLabelA, dstLabelA);
                std::swap(srcLabelB, dstLabelB);
            }

            Q_ASSERT(!srcLabelA && !srcLabelB);


            // TODO: precalculate!
            const int k  = 2 * (map.m_mainRect.width() + map.m_mainRect.height());

            float value = 0.0;

            if (dstLabelA) {
                map.m_aAccessor->moveTo(src.x, src.y);
                const int i0 = *((quint8*)map.m_aAccessor->rawDataConst());
                value = i0 / 255.0 * k;

            } else if (dstLabelB) {
                map.m_bAccessor->moveTo(src.x, src.y);
                const int i0 = *((quint8*)map.m_bAccessor->rawDataConst());
                value = i0 / 255.0 * k;

            } else {
                map.m_mainAccessor->moveTo(src.x, src.y);
                memcpy(map.m_srcPixelBuf.data(), map.m_mainAccessor->rawDataConst(), map.m_pixelSize);
                map.m_mainAccessor->moveTo(dst.x, dst.y);

                //const quint8 diff = map.m_colorSpace->differenceA((quint8*)map.m_srcPixelBuf.data(), map.m_mainAccessor->rawDataConst());
                //const quint8 i0 = map.m_colorSpace->intensity8((quint8*)map.m_srcPixelBuf.data());
                //const quint8 i1 = map.m_colorSpace->intensity8(map.m_mainAccessor->rawDataConst());

                const quint8 i0 = *((quint8*)map.m_srcPixelBuf.data());
                const quint8 i1 = *map.m_mainAccessor->rawDataConst();

                const quint8 diff = qAbs(i1 - i0);

                const qreal diffPenalty = qBound(0.0, qreal(diff) / 10.0, 1.0);
                const qreal intensityPenalty = 1.0 - i1 / 255.0;

                const qreal totalPenalty = qMax(diffPenalty, intensityPenalty);

                value = 1.0 + k * (1.0 - pow2(totalPenalty));
            }

            return value;
        }

    KisLazyFillGraph& graph() {
        return m_graph;
    }

private:
    KisPaintDeviceSP m_mainImage;
    KisPaintDeviceSP m_aLabelImage;
    KisPaintDeviceSP m_bLabelImage;

    QRect m_mainRect;
    QRect m_aLabelRect;
    QRect m_bLabelRect;

    const KoColorSpace *m_colorSpace;
    int m_pixelSize;
    KisRandomConstAccessorSP m_mainAccessor;
    KisRandomConstAccessorSP m_aAccessor;
    KisRandomConstAccessorSP m_bAccessor;
    QByteArray m_srcPixelBuf;

    KisLazyFillGraph m_graph;
};

template <class Graph>
ComplexCapacityMap<Graph>
MakeComplexCapacityMap(KisPaintDeviceSP mainImage,
                       KisPaintDeviceSP aLabelImage,
                       KisPaintDeviceSP bLabelImage) {
    return KisLazyFillCapacityMap(mainImage, aLabelImage, bLabelImage);
}

#endif /* __KIS_LAZY_FILL_CAPACITY_MAP_H */
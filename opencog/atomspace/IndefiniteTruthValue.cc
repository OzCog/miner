/*
 * opencog/atomspace/IndefiniteTruthValue.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Fabricio Silva <fabricio@vettalabs.com>
 *            Welter Silva <welter@vettalabs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "IndefiniteTruthValue.h"

#include <opencog/util/platform.h>
#include <opencog/util/exceptions.h>

#define W() getU()-getL();

using namespace opencog;

float IndefiniteTruthValue::DEFAULT_CONFIDENCE_LEVEL = 0.9f;
float IndefiniteTruthValue::DEFAULT_K = 2.0f;

void IndefiniteTruthValue::init(float l, float u, float c)
{
    mean = (l + u) / 2;
    L = l;
    U = u;
    confidenceLevel = c;
    diff = 0.0f;
    symmetric = true;
}

void IndefiniteTruthValue::copy(const IndefiniteTruthValue& source)
{
    mean = source.mean;
    L = source.L;
    U = source.U;
    confidenceLevel = source.confidenceLevel;
    diff = source.diff;
    symmetric = source.symmetric;
}

IndefiniteTruthValue::IndefiniteTruthValue()
{
    init();
}

IndefiniteTruthValue::IndefiniteTruthValue(float l, float u, float c)
{
    init(l, u, c);
}

IndefiniteTruthValue::IndefiniteTruthValue(IndefiniteTruthValue const& source)
{
    copy(source);
}

IndefiniteTruthValue* IndefiniteTruthValue::clone() const
{
    return new IndefiniteTruthValue(*this);
}

IndefiniteTruthValue& IndefiniteTruthValue::operator=(const TruthValue & rhs) throw (RuntimeException)
{
    const IndefiniteTruthValue* tv = dynamic_cast<const IndefiniteTruthValue*>(&rhs);
    if (tv) {
        if (tv != this) { // check if this is the same object first.
            copy(*tv);
        }
    } else {
#if 0
        // The following line was causing a compilation error on MSVC...
        throw RuntimeException(TRACE_INFO, "Cannot assign a TV of type '%s' to one of type '%s'\n",
                               typeid(rhs).name(), typeid(*this).name());
#else
        throw RuntimeException(TRACE_INFO, "Invalid assignment of a IndefiniteTV object\n");
#endif
    }
    return *this;
}

bool IndefiniteTruthValue::operator==(const TruthValue& rhs) const
{
    return false; // XXX Implement me!
}

float IndefiniteTruthValue::getL() const
{
    return L;
}
float IndefiniteTruthValue::getU() const
{
    return U;
}
float IndefiniteTruthValue::getDiff() const
{
    return diff;
}
float IndefiniteTruthValue::getConfidenceLevel() const
{
    return confidenceLevel;
}

vector<float*> IndefiniteTruthValue::getFirstOrderDistribution() const
{
    return firstOrderDistribution;
}

float IndefiniteTruthValue::getU_() const
{
    float u = U + diff;
// return (u > 1.0)?1.0f:u;
    return u;
}
float IndefiniteTruthValue::getL_() const
{
    float l = L - diff;
// return (l < 0.0)?0.0f:l;
    return l;
}

void IndefiniteTruthValue::setL(float l)
{
    this->L = l;
}

void IndefiniteTruthValue::setU(float u)
{
    this->U = u;
}

void IndefiniteTruthValue::setConfidenceLevel(float c)
{
    this->confidenceLevel = c;
}

void IndefiniteTruthValue::setDiff(float diff)
{
    this->diff = diff;
}

void IndefiniteTruthValue::setFirstOrderDistribution(vector<float*> v)
{
    this->firstOrderDistribution = v;
}

TruthValueType IndefiniteTruthValue::getType() const
{
    return INDEFINITE_TRUTH_VALUE;
}

void IndefiniteTruthValue::setMean(float m)
{
    mean = m;
}

float IndefiniteTruthValue::getMean() const
{
    return mean;
}

float IndefiniteTruthValue::getCount() const
{
    float W = W();
    W = max(W, 0.000001f); // to avoid division by zero
    float c = (DEFAULT_K * (1 - W) / W);
    return c;
}

float IndefiniteTruthValue::getConfidence() const
{
    float count = getCount();
    return (count / (count + DEFAULT_K));
}

bool IndefiniteTruthValue::isSymmetric() const
{
    return symmetric;
}

std::string IndefiniteTruthValue::toString() const
{
    char buf[1024];
    sprintf(buf, "[%f,%f,%f,%f,%f,%d]", mean, L, U, confidenceLevel, diff, symmetric);
    return buf;
}

IndefiniteTruthValue* IndefiniteTruthValue::fromString(const char* tvStr)
{
    float m, l, u, c, d;
    int s;
    sscanf(tvStr, "[%f,%f,%f,%f,%f,%d]", &m, &l, &u, &c, &d, &s);
    //printf("IndefiniteTruthValue::fromString(%s) => mean = %f, L = %f, U = %f, confLevel = %f, diff = %f, symmetric = %d\n", tvStr, m, l, u, c, d, s);
    IndefiniteTruthValue* result = new IndefiniteTruthValue(l, u, c);
    result->setDiff(d);
    result->symmetric = s != 0;
    result->setMean(m);
    return result;
}

float IndefiniteTruthValue::toFloat() const
{
    return getMean();
}


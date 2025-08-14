/*
 * typedparams.h: Helpers for translating virTypedParameter to/from assoc arrays
 *
 * See COPYING for the license of this software
 */

#pragma once

#include <libvirt/libvirt.h>

typedef struct {
    const char *name;
    virTypedParameterType type;
} virPHPTypedParamsHint;

int
parseTypedParameter(zval *zend_params,
                    virTypedParameterPtr *paramsRet,
                    int *nparamsRet,
                    virPHPTypedParamsHint *hint,
                    int nhints);

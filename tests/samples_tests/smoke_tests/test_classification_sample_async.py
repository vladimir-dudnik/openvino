"""
 Copyright (C) 2018-2021 Intel Corporation
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
      http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""
import os
import pytest
import re
import sys
import logging as log
from common.samples_common_test_clas import SamplesCommonTestClass
from common.samples_common_test_clas import get_tests

log.basicConfig(format="[ %(levelname)s ] %(message)s", level=log.INFO, stream=sys.stdout)

test_data_fp32 = get_tests(cmd_params={'i': [os.path.join('227x227', 'dog.bmp')],
                                       'm': [os.path.join('squeezenet1.1', 'caffe_squeezenet_v1_1_FP32_batch_1_seqlen_[1]_v10.xml')],
                                       'sample_type': ['C++','Python'],
                                       'batch': [1, 2, 4],
                                       'd': ['CPU']},
                           use_device=['d']
                           )

test_data_fp16 = get_tests(cmd_params={'i': [os.path.join('227x227', 'dog.bmp')],
                                       'm': [os.path.join('squeezenet1.1', 'caffe_squeezenet_v1_1_FP16_batch_1_seqlen_[1]_v10.xml')],
                                       'sample_type': ['C++','Python'],
                                       'batch': [1, 2, 4],
                                       'd': ['CPU']}, 
                           use_device=['d']
                           )


class TestClassification(SamplesCommonTestClass):
    @classmethod
    def setup_class(cls):
        cls.sample_name = 'classification_sample_async'
        super().setup_class()

    @pytest.mark.parametrize("param", test_data_fp32)
    def test_classification_sample_async_fp32(self, param):
        _check_output(self, param)

    @pytest.mark.parametrize("param", test_data_fp16)
    def test_classification_sample_async_fp16(self, param):
        _check_output(self, param)


def _check_output(self, param):
    """
    Classification_sample_async has functional and accuracy tests.
    For accuracy find in output class of detected on image object
    """
    # Run _test function, that returns stdout or 0.
    stdout = self._test(param)
    if not stdout:
        return 0
    stdout = stdout.split('\n')
    is_ok = 0
    for line in range(len(stdout)):
        if re.match("\d+ +\d+.\d+$", stdout[line].replace('[ INFO ]', '').strip()) is not None:
            if is_ok == 0:
                is_ok = True
            top1 = stdout[line].replace('[ INFO ]', '').strip().split(' ')[0]
            top1 = re.sub("\D", "", top1)
            if '215' not in top1:
                is_ok = False
                log.info("Detected class {}".format(top1))
            break
    assert is_ok, "Wrong top1 class"
    log.info('Accuracy passed')

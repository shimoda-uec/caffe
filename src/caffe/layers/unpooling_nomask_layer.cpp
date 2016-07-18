/*************************************************************************
	> File Name: unpooling_layer.cpp
	> Author:
	> Mail:
	> Created Time: 2015�N11��16�� ������ 15?42��28�b
 ************************************************************************/

#include <algorithm>
#include <cfloat>
#include <vector>


#include "caffe/util/math_functions.hpp"
#include "caffe/layers/unpooling_nomask_layer.hpp"

namespace caffe {

using std::min;
using std::max;

template <typename Dtype>
void UnpoolingNomaskLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
  PoolingParameter pool_param = this->layer_param_.pooling_param();
  global_pooling_ = pool_param.global_pooling();
  if (global_pooling_) {
  } else {
    if (pool_param.has_kernel_size()) {
      kernel_h_ = kernel_w_ = pool_param.kernel_size();
    } else {
      kernel_h_ = pool_param.kernel_h();
      kernel_w_ = pool_param.kernel_w();
    }
  }
  if (!pool_param.has_pad_h()) {
    pad_h_ = pad_w_ = pool_param.pad();
  } else {
    pad_h_ = pool_param.pad_h();
    pad_w_ = pool_param.pad_w();
  }
  if (!pool_param.has_stride_h()) {
    stride_h_ = stride_w_ = pool_param.stride();
  } else {
    stride_h_ = pool_param.stride_h();
    stride_w_ = pool_param.stride_w();
  }


}


template <typename Dtype>
void UnpoolingNomaskLayer<Dtype>::Reshape(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
  channels_ = bottom[0]->channels();
  height_ = bottom[0]->height();
  width_ = bottom[0]->width();
  if (global_pooling_) {
    kernel_h_ = bottom[0]->height();
    kernel_w_ = bottom[0]->width();
  }
  pooled_height_ = bottom[1]->height();
  pooled_width_ = bottom[1]->width();
  top[0]->Reshape(bottom[0]->num(), channels_, pooled_height_, pooled_width_);
}



template <typename Dtype>
void UnpoolingNomaskLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
  //top[0]->Reshape(1, channels_, pooled_height_, pooled_width_);
  //const Dtype* eachn = bottom[1]->cpu_data();//conv value

  const Dtype *bottom_data = bottom[0]->cpu_data();
  Dtype *top_data = top[0]->mutable_cpu_data();
  // Init the top blob to be all zero since only special places
  // wouldn't be zero then.
  caffe_set(top[0]->count(), Dtype(0), top_data);
  switch (this->layer_param_.pooling_param().pool()) {
  case PoolingParameter_PoolMethod_MAX:
      //for (int n = 0; n < 1; ++n) {
      for (int n = 0; n < bottom[0]->num(); ++n) {
       for (int c = 0; c < channels_; ++c) {
    		for (int ph = 0; ph < height_; ++ph) {
    		  for (int pw = 0; pw <width_; ++pw) {
    		    int hstart = ph * stride_h_ - pad_h_;
    		    int wstart = pw * stride_w_ - pad_w_;
    		    int hend = min(hstart + kernel_h_, pooled_height_);
    		    int wend = min(wstart + kernel_w_, pooled_width_);
    		    hstart = max(hstart, 0);
    		    wstart = max(wstart, 0);
    		    const int pool_index = ph * width_ + pw;
    		    for (int h = hstart; h < hend; ++h) {
    		      for (int w = wstart; w < wend; ++w) {
                        const int index = h * pooled_width_ + w;
    					top_data[index] += bottom_data[ pool_index];
    		       }
    		     }
    		  }
    		}
              //switch to next channel
              top_data += top[0]->offset(0, 1);
              bottom_data += bottom[0]->offset(0, 1);
              //mask += bottom[0]->offset(0, 1);
      }
    }

      break;
  case PoolingParameter_PoolMethod_AVE:
      NOT_IMPLEMENTED;
      break;
  case PoolingParameter_PoolMethod_STOCHASTIC:
      NOT_IMPLEMENTED;
      break;
  }

}
template <typename Dtype>
void UnpoolingNomaskLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
}


#ifdef CPU_ONLY
STUB_GPU(UnpoolingNomaskLayer);
#endif


INSTANTIATE_CLASS(UnpoolingNomaskLayer);
REGISTER_LAYER_CLASS(UnpoolingNomask);
} //namespace caffe

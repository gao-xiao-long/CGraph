/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: GElementRepository.cpp
@Time: 2023/6/24 17:51
@Desc: 
***************************/

#include "GElementRepository.h"
#include "GNode/GNodeInclude.h"

CGRAPH_NAMESPACE_BEGIN

CVoid GElementRepository::insert(GElementPtr ptr) {
    CGRAPH_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    elements_.insert(ptr);
}


CBool GElementRepository::find(GElementPtr ptr) const {
    CGRAPH_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    return elements_.find(ptr) != elements_.end();
}


GElementRepositoryPtr GElementRepository::setThreadPool(UThreadPoolPtr ptr) {
    CGRAPH_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    for (auto& cur : this->elements_) {
        cur->setThreadPool(ptr);
    }
    return this;
}


CStatus GElementRepository::setup() {
    CGRAPH_FUNCTION_BEGIN
    // 一旦执行，全部设置为 normal状态
    status = pushAllState(GElementState::NORMAL);
    CGRAPH_FUNCTION_END
}


CStatus GElementRepository::reset() {
    CGRAPH_FUNCTION_BEGIN
    for (auto& cur : async_elements_) {
        status += cur->getAsyncResult();
        if (cur->isTimeout()) {
            // 如果是超时的状态，则恢复原样
            cur->cur_state_.store(GElementState::NORMAL);
        }
    }

    CGRAPH_FUNCTION_END
}


CStatus GElementRepository::pushAllState(const GElementState& state) {
    CGRAPH_FUNCTION_BEGIN
    if (cur_state_ == state) {
        return status;    // 避免重复赋值
    }

    cur_state_ = state;    // 记录当前的状态信息
    for (auto& cur : elements_) {
        cur->cur_state_.store(state);
        if (GElementState::YIELD != state) {
            // 目前仅非yield状态，需要切换的。如果一直处于 yield状态，是不需要被通知的
            cur->yield_cv_.notify_one();
        }
    }
    CGRAPH_FUNCTION_END
}


CBool GElementRepository::isCancelState() const {
    /**
     * 因为每次执行的时候，都需要判断一下这个状态是否为 cancel
     * 且理论上不会出现多线程问题
     * 故这一层的 cur_state_ 就不设置为atomic类型的了
     */
    return GElementState::CANCEL == cur_state_;
}


CStatus GElementRepository::init() {
    CGRAPH_FUNCTION_BEGIN
    for (auto& element : elements_) {
        /**
         * 以下几种情况，需要到最后，再确认是否执行ok的
         * 1. 异步节点信息
         * 2. 包含超时逻辑的信息
         */
        CGRAPH_ASSERT_NOT_NULL(element)
        if (element->isAsync()) {
            async_elements_.emplace(element);
        }
    }
    CGRAPH_FUNCTION_END
}


CStatus GElementRepository::destroy() {
    CGRAPH_FUNCTION_BEGIN
    /**
     * destroy的时候，恢复create的状态，确保再次轮训的时候正常
     * 理论上，cancel状态仅会出现在极短的时间段之内，
     * 当程序 cancel完成之后，就会重新恢复 CREATE的状态
     * 问题详见: https://github.com/ChunelFeng/CGraph/issues/153
     */
    status = pushAllState(GElementState::CREATE);
    CGRAPH_FUNCTION_END
}


CStatus GElementRepository::run() {
    CGRAPH_NO_SUPPORT
}


GElementRepository::~GElementRepository() {
    // 删除所有内部的element信息
    for (GElementPtr element : elements_) {
        CGRAPH_DELETE_PTR(element)
    }
}


CGRAPH_NAMESPACE_END
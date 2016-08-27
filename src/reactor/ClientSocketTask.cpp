#include "aliens/reactor/ClientSocketTask.h"

using aliens::async::ErrBack;

namespace aliens { namespace reactor {

void ClientSocketTask::EventHandler::setTask(ClientSocketTask *task) {
  task_ = task;
}

ClientSocketTask* ClientSocketTask::EventHandler::getTask() {
  return task_;
}

void ClientSocketTask::EventHandler::write(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
  task_->write(std::move(buff), std::forward<ErrBack>(cb));
}

void ClientSocketTask::EventHandler::readInto(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
  task_->readInto(std::move(buff), std::forward<ErrBack>(cb));
}

void ClientSocketTask::readInto(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
  LOG(INFO) << "readInto()";
}

void ClientSocketTask::write(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
  LOG(INFO) << "write()";
}

ClientSocketTask::ClientSocketTask(TCPSocket &&sock, EventHandler *handler)
  : sock_(std::forward<TCPSocket>(sock)), handler_(handler){
  handler_->setTask(this);
}

void ClientSocketTask::onEvent() {
  LOG(INFO) << "ClientSocketTask onEvent";
  // doRead();
}

void ClientSocketTask::onError() {
  LOG(INFO) << "ClientSocketTask onError";
}

int ClientSocketTask::getFd() {
  return sock_.getFdNo();
}

}} // aliens::reactor

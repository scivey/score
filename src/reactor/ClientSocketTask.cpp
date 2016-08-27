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
  auto b2 = buff.release();
  ::write(getFd(), b2->body(), b2->currentLen());
}

ClientSocketTask::ClientSocketTask(TCPSocket &&sock, EventHandler *handler)
  : sock_(std::forward<TCPSocket>(sock)), handler_(handler){
  handler_->setTask(this);
}

void ClientSocketTask::onReadable() {
  LOG(INFO) << "ClientSocketTask onReadable";
  handler_->onReadable();
}

void ClientSocketTask::onWritable() {
  LOG(INFO) << "ClientSocketTask onWritable";
  handler_->onWritable();
}

void ClientSocketTask::onError() {
  LOG(INFO) << "ClientSocketTask onError";
}

int ClientSocketTask::getFd() {
  return sock_.getFdNo();
}

}} // aliens::reactor

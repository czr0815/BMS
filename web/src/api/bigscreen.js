import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getScreenOrderStat() {
  return request({
    url: '/api/v1/getScreenOrderStat',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getScreenWarningStat() {
  return request({
    url: '/api/v1/getScreenWarningStat',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getScreenOverallStat() {
  return request({
    url: '/api/v1/getScreenOverallStat',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getScreenStationStat() {
  return request({
    url: '/api/v1/getScreenStationStat',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getScreenBatteryStat() {
  return request({
    url: '/api/v1/getScreenBatteryStat',
    method: 'get',
    params: {
      token: token
    }
  })
}
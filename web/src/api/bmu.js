import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getBMUInfos() {
  return request({
    url: '/api/v1/getBMUInfos',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getBMUDataStat(bmuId) {
  return request({
      url: '/api/v1/getBMUDataStat',
      method: 'get',
      params: {
        bmuId: bmuId,
        token: token
      }
  })
}

export function getBMUWarningStat(bmuId) {
  return request({
      url: '/api/v1/getBMUWarningStat',
      method: 'get',
      params: {
        bmuId: bmuId,
        token: token
      }
  })
}

export function getBMUWarningCalendar(bmuId) {
  return request({
      url: '/api/v1/getBMUWarningCalendar',
      method: 'get',
      params: {
        bmuId: bmuId,
        token: token
      }
  })
}

export function getBMUSOCStat(bmuId) {
  return request({
      url: '/api/v1/getBMUSOCStat',
      method: 'get',
      params: {
        bmuId: bmuId,
        token: token
      }
  })
}

export function getBMUOutlineStat(bmuId) {
  return request({
    url: '/api/v1/getBMUOutlineStat',
    method: 'get',
    params: {
      bmuId: bmuId,
      token: token
    }
  })
}

export function getSingleTable(bmuId) {
  return request({
    url: '/api/v1/getBMUSingles',
    method: 'get',
    params: {
      bmuId: bmuId,
      token: token
    }
  })
}

export function updateBMUInfo(bmuId, bmuForm) {
  return request({
    url: '/api/v1/updateBMU/'+bmuId,
    method: 'put',
    params: {
      token: token
    },
    data: bmuForm
  })
}

export function addBMU(bmuCreate) {
  bmuCreate.id += ""
  bmuCreate.bcuId += ""
  bmuCreate.batteryType += ""
  bmuCreate.batteryNum += ""
  return request({
    url: '/api/v1/addBMU',
    method: 'post',
    params: {
      token: token
    },
    data: bmuCreate
  })
}
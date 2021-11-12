import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getBCUInfos() {
  return request({
    url: '/api/v1/getBCUInfos',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function getBCURank(stationId) {
  return request({
    url: '/api/v1/getBCURank',
    method: 'get',
    params: {
      stationId: stationId,
      token: token
    }
  })
}

export function getBCUDataStat(bcuId) {
  return request({
      url: '/api/v1/getBCUDataStat',
      method: 'get',
      params: {
        bcuId: bcuId,
        token: token
      }
  })
}

export function getBCUSystemStat(bcuId) {
  return request({
      url: '/api/v1/getBCUSystemStat',
      method: 'get',
      params: {
        bcuId: bcuId,
        token: token
      }
  })
}

export function getBCUWarningStat(bcuId) {
  return request({
      url: '/api/v1/getBCUWarningStat',
      method: 'get',
      params: {
        bcuId: bcuId,
        token: token
      }
  })
}

export function getBCUSingleStat(bcuId) {
  return request({
      url: '/api/v1/getBCUSingleStat',
      method: 'get',
      params: {
        bcuId: bcuId,
        token: token
      }
  })
}

export function getBCUSlaveTable(bcuId) {
  return request({
    url: '/api/v1/getBCUSlaves',
    method: 'get',
    params: {
      bcuId: bcuId,
      token: token
    }
  })
}

export function updateBCUInfo(bcuId, bcuForm) {
  return request({
    url: '/api/v1/updateBCU/'+bcuId,
    method: 'put',
    params: {
      token: token
    },
    data: bcuForm
  })
}

export function addBCU(bcuCreate) {
  bcuCreate.id += ""
  bcuCreate.stationId += ""
  return request({
    url: '/api/v1/addBCU',
    method: 'post',
    params: {
      token: token
    },
    data: bcuCreate
  })
}
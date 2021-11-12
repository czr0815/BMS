import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getOrderStationStat() {
  return request({
      url: '/api/v1/getOrderStationStat',
      method: 'get',
      params: {
        token: token
      }
  })
}

export function getOrderTimeStat() {
  return request({
      url: '/api/v1/getOrderTimeStat',
      method: 'get',
      params: {
        token: token
      }
  })
}

export function getOrders(orderForum) {
  return request({
    url: 'api/v1/getOrders?device='+orderForum.device+'&deviceId='+orderForum.deviceId+'&orderType='+orderForum.valueType+'&startTime='+orderForum.timeRange[0]+'&endTime='+orderForum.timeRange[1],
    method: 'get',
    params: {
      token: token
    }
  })
}

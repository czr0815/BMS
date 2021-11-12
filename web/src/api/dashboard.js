import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getDashboardOrder() {
    return request({
        url: '/api/v1/getDashboardOrder',
        method: 'get',
        params: {
          token: token
        }
    })
}

export function getDashboardStatus() {
    return request({
        url: 'api/v1/getDashboardStatus',
        method: 'get',
        params: {
          token: token
        }
    })
}

export function getUserInfo() {
    return request({
        url: '/api/v1/getUserInfo',
        method: "get",
        params: {
          token: token
        }
    })
}
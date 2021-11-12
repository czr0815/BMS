import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function getEmployments() {
  return request({
    url: 'api/v1/getEmployments',
    methods: 'get',
    params: {
      token: token
    }
  })
}
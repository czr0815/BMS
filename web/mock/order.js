const WeekData = {
    xAxis: ["周一", "周二", "周三", "周四", "周五", "周六", "周日"],
    curWeek: [150, 170, 160, 180, 170, 190, 180],
    lastWeek: [150, 140, 130, 160, 150, 140, 160]
}

const DayData = {
    xAxis: ["0-4", "4-8", "8-12", "12-16", "16-20", "20-24"],
    curDay: [50, 80, 130, 160, 250, 140, 70]
}

const StatusData = [
    { name: '已完成', value: 30 },
    { name: '进行中', value: 30 },
    { name: '无效', value: 30 },
]

const TypeData = {
    "微信支付": 13,
    "支付宝": 12,
    "银联支付": 30
}

const BmuData = [
    { name: '1', value: 10 },
    { name: '2', value: 20 },
    { name: '3', value: 30 },
    { name: '4', value: 30 },
    { name: '5', value: 30 },
    { name: '6', value: 30 },
    { name: '7', value: 30 },
    { name: '8', value: 30 },
    { name: '9', value: 30 },
    { name: '10', value: 30 },
]

const orderTable = [
    {
        id: '00000000000000000001',
        type: "微信",
        status: "已完成",
        left: 37,
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '1号',
        bcu: '1号',
        bmu: '1号'
    }, {
        id: '00000000000000000002',
        type: "支付宝",
        status: "进行中",
        left: 99,
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: 'ID-002',
        bcu: 'ID-002',
        bmu: 'ID-002'
    }, {
        id: '00000000000000000003',
        type: "银联",
        status: "无效",
        left: 54,
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: 'ID-003',
        bcu: 'ID-003',
        bmu: 'ID-003'
    }, {
        id: '00000000000000000003',
        type: "银联",
        status: "无效",
        left: 54,
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '3号',
        bcu: '3号',
        bmu: '3号'
    } , {
        id: '00000000000000000003',
        type: "银联",
        status: "无效",
        left: 54,
        starttime: '2021-06-12 15:43',
        endtime: '2021-06-12 15:43',
        bsu: '3号',
        bcu: '3号',
        bmu: '3号'
    } 
]

module.exports = [


    {
        url: '/order/orderWeek',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: WeekData
            }
        }
    },

    {
        url: '/order/orderDay',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: DayData
            }
        }
    },

    {
        url: '/order/orderSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: StatusData
            }
        }
    },

    {
        url: '/order/orderType',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: TypeData
            }
        }
    },

    {
        url: '/order/orderBmu',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: BmuData
            }
        }
    },

    {
        url: '/order/orders',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: orderTable
            }
        }
    },

]
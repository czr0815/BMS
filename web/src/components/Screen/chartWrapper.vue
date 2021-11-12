<template>
    <div class="screen-chart-wrapper">
        <div class="title">
            {{title}}<span>{{desc}}</span>
            <div class="title-circle-out"/>
            <div class="title-circle-in"/>
        </div>
        <div class="content">
            <slot></slot>
        </div>
    </div>
</template>

<script>
export default {
    props: {
        title: {
            type: String,
            default: "screen title"
        },
        desc: {
            type: String,
            default: new Date().getFullYear()+"/"+new Date().getMonth()+"/"+new Date().getDay()+" "+new Date().getHours()+":"+new Date().getMinutes()
        }
    }
}
</script>

<style lang="scss" scoped>
.screen-chart-wrapper {
    margin-bottom: 15px;
    .title {
        position: relative;
        padding-left: 15px;
        height: 40px;
        line-height: 40px;
        background-image: url("../../assets/blue-bg.png");
        background-size: 100% 100%;
        .title-circle-out {
            position: absolute;
            content: " ";
            width: 10px;
            height: 10px;
            top: -5px;
            left: calc(-100% / 50);
            border-radius: 50%;
            background-color: #020507;
            border: 3px solid #009FFB;
        }

        span {
            margin-left: 15px;
            opacity: 0.7;
            font-size: 14px;
        }
    }
}
</style>
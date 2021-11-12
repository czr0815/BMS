<template>
  <div class="radar-wrapper">
    <div class="radar">
      <div class="pointer"></div>
    </div>
    <div class="content">
      <div class="value">
        <countTo :endVal="value" :duration="3000"></countTo>
      </div>
      <div class="name">{{ name }}</div>
    </div>
  </div>
</template>

<script>
import countTo from "vue-count-to";
export default {
  components: { countTo },
  props: {
    value: {
      type: Number,
      default: 50,
    },
    name: {
      type: String,
      default: "默认名称",
    },
  },
};
</script>

<style lang="scss" scoped>
$size: 80px;
.radar {
  background: url("../../assets/radar.png") no-repeat 50% 50%;
  width: $size;
  height: $size;
  position: relative;
  background-size: $size $size;
}

.radar .pointer {
  position: absolute;
  z-index: 1024;
  left: 10.5820106%;
  right: 10.5820106%;
  top: 10.5820106%;
  bottom: 50%;
  will-change: transform;
  transform-origin: 50% 100%;
  border-radius: 50% 50% 0 0 / 100% 100% 0 0;
  background-image: linear-gradient(
    135deg,
    rgba(0, 136, 212, 0.8) 0%,
    rgba(0, 0, 0, 0.02) 70%,
    rgba(0, 0, 0, 0) 100%
  );
  clip-path: polygon(
    100% 0,
    100% 10%,
    //控制扇形角度 100% => 135deg
    50% 100%,
    0 100%,
    0 0
  );

  animation: rotate360 3s infinite linear;
}

.radar .pointer:after {
  content: "";
  position: absolute;
  width: 50%;
  bottom: -2px;
  border-top: 3px solid rgba(0, 136, 212, 0.8);
  box-shadow: 0 0 3px rgba(0, 136, 212, 0.6);
  border-radius: 9px;
}

.radar-wrapper {
  display: flex;
  align-items: center;
  padding: 0 15px;
  .content {
    margin-left: 15px;
  }
  .value {
      font-size: 18px;
  }
  .name {
    color: #ccc;
    opacity: 0.6;
    margin-top: 5px;
    font-size: 14px;
  }
}

@keyframes rotate360 {
  0% {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(-360deg);
  }
}
</style>
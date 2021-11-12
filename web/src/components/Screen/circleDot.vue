<template>
  <div class="circle-dot-wrapper">
    <div class="circle-animation">
      <svg
        version="1.1"
        id="L3"
        xmlns="http://www.w3.org/2000/svg"
        xmlns:xlink="http://www.w3.org/1999/xlink"
        x="0px"
        y="0px"
        viewBox="0 0 100 100"
        enable-background="new 0 0 0 0"
        xml:space="preserve"
      >
        <circle
          fill="none"
          :stroke="color"
          stroke-width="4"
          cx="50"
          cy="50"
          r="44"
          style="opacity: 0.5"
        />
        <circle
          :fill="color"
          stroke="#020507"
          stroke-width="10"
          cx="8"
          cy="54"
          r="8"
        >
          <animateTransform
            attributeName="transform"
            :dur="`4s`"
            type="rotate"
            :from="`${offset * 45} 50 52`"
            :to="`${offset * 45 + 360} 50 52`"
            repeatCount="indefinite"
          />
        </circle>
      </svg>
      <div class="pie">
        <div class="slice-1 slice" :style="sliceStyle"></div>
        <div class="slice-2 slice" :style="sliceStyle2"></div>
      </div>
    </div>
    <div>
      <div class="value">
        <countTo :startVal="0" :endVal="value" :duration="3000" />
      </div>
      <div class="title">
        {{ title }}
      </div>
      <div class="desc" v-if="desc.length > 0">
        {{ desc }}
      </div>
    </div>
  </div>
</template>

<script>
import countTo from "vue-count-to";
export default {
  components: { countTo },
  props: {
    offset: {
      type: Number,
      default: 0,
    },
    value: {
      type: Number,
      default: 10000,
    },
    title: {
      type: String,
      default: "default title",
    },
    desc: {
      type: String,
      default: "",
    },
    color: {
      type: String,
      default: "#FF0088",
    },
  },
  data() {
    return {
      sliceStyle: `transform: rotate(-${this.offset * 60}deg); background: ${
        this.color
      }`,
      sliceStyle2: `transform: rotate(-${
        this.offset * 60 + 60
      }deg); background: ${this.color}`,
    };
  },
};
</script>

<style lang="scss" scoped>
.circle-dot-wrapper {
  display: flex;
  align-items: center;
  .circle-animation {
    position: relative;
    margin-right: 10px;
  }
  svg {
    width: 70px;
    height: 70px;
  }

  .pie {
    position: absolute;
    top: 16px;
    left: 16px;
    padding: 0;
    width: 36px;
    height: 36px;
    border-radius: 100%;
    overflow: hidden;
    transform: rotate(0deg); /*针对mac safari浏览器兼容*/
    animation:turn 4s linear infinite;  
  }
  .slice {
    overflow: hidden;
    position: absolute;
    top: 0;
    right: 0;
    width: 50%;
    height: 50%;
    transform-origin: 0% 100%;
    background: #ff0088;
  }
  .slice-1 {
    transform: skewY(0deg);
  }
  .slice-2 {
    transform: skewY(-30deg);
  }
  .value {
    font-size: 20px;
    white-space: nowrap;
  }

  .title {
    white-space: nowrap;
    color: gray;
    margin: 5px 0px;
  }

  .desc {
    color: #009ffb;
    font-size: 14px;
  }
}

@keyframes turn {
  0% {
    -webkit-transform: rotate(360deg);
  }
  25% {
    -webkit-transform: rotate(270deg);
  }
  50% {
    -webkit-transform: rotate(180deg);
  }
  75% {
    -webkit-transform: rotate(90deg);
  }
  100% {
    -webkit-transform: rotate(00deg);
  }
}
</style>
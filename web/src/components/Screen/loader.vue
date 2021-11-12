<template>
  <div class="circle-wrapper">
    <div class="loader" title="0">
      <svg viewBox="-10 -10 120 120">
        <path
          d="
        M 50 50
        m 0 -46
        a 46 46 0 1 1 0 92
        a 46 46 0 1 1 0 -92
        "
          :stroke="color"
          stroke-width="7.9"
          fill="none"
          style="stroke-dasharray: 289.027px, 289.027px; stroke-dashoffset: 0px; opacity: 0.5"
        ></path>
        <path
          d="
        M 50 50
        m 0 -46
        a 46 46 0 1 1 0 92
        a 46 46 0 1 1 0 -92
        "
          :stroke="color"
          fill="none"
          stroke-linecap="round"
          stroke-width="14.9"
          :style="`stroke-dasharray: ${
            (289.027 * value) / 100
          }px, 289.027px; stroke-dashoffset: 0px; transition: stroke-dasharray 0.6s ease 0s, stroke 0.6s ease 0s;`"
        >
        <animateTransform
            attributeName="transform"
            :dur="`4s`"
            type="rotate"
            :from="`${rank * 45} 50 50`"
            :to="`${rank * 45 + 360} 50 50`"
            repeatCount="indefinite"
          />
        </path>
      </svg>
    </div>
    <div class="content">
      <div class="value">{{ value }}%</div>
      <div class="name">{{ name }}</div>
      <div class="rank">{{ rank }}</div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    value: {
      type: Number,
      default: 50,
    },
    name: {
      type: String,
      default: "默认名称",
    },
    rank: {
      type: Number,
      default: 0,
    },
    color: {
      type: String,
      default: "#27CCF8",
    },
  },
};
</script>

<style lang="scss" scoped>
.circle-wrapper {
  display: flex;
  align-items: flex-end;
  margin: 10px 15px;
  .loader {
    height: 40px;
    width: 40px;
  }

  .loader-content:after {
    border: 5px solid #ccc;
  }

  .content {
    margin-left: 10px;
    flex-grow: 1;
    position: relative;
    padding-bottom: 3px;
    border-bottom: 1px dashed #5687c3;
  }
  .name,
  .rank {
    color: #ccc;
    opacity: 0.6;
  }
  .name {
    margin-top: 5px;
    font-size: 14px;
  }
  .rank {
    position: absolute;
    right: 0;
    top: 10px;
  }
}

@keyframes spin {
  0% {
    -webkit-transform: rotate(0deg);
    -ms-transform: rotate(0deg);
    -o-transform: rotate(0deg);
    transform: rotate(0deg);
  }

  100% {
    -webkit-transform: rotate(360deg);
    -ms-transform: rotate(360deg);
    -o-transform: rotate(360deg);
    transform: rotate(360deg);
  }
}
</style>
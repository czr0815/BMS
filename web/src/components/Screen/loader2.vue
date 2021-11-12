<template>
  <div class="circle-wrapper-2">
    <div class="loader">
      <svg viewBox="-10 -10 120 120">
        <circle :fill="color" cx="50" cy="50" r="15" />
        <path
          d="
        M 50 50
        m 0 -46
        a 46 46 0 1 1 0 92
        a 46 46 0 1 1 0 -92
        "
          :stroke="color"
          stroke-width="4"
          fill="none"
          style="
            stroke-dasharray: 289.027px, 289.027px;
            stroke-dashoffset: 0px;
            opacity: 0.5;
          "
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
          stroke-width="7.9"
          :style="`stroke-dasharray: 31px, 289.027px; stroke-dashoffset: 0px; transition: stroke-dasharray 0.6s ease 0s, stroke 0.6s ease 0s;`"
        >
          <animateTransform
            attributeName="transform"
            :dur="`4s`"
            type="rotate"
            :from="`${offset * 45 + 200} 50 50`"
            :to="`${offset * 45 + 360 + 200} 50 50`"
            repeatCount="indefinite"
          />
        </path>
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
          stroke-width="7.9"
          :style="`stroke-dasharray: 62px, 289.027px; stroke-dashoffset: 0px; transition: stroke-dasharray 0.6s ease 0s, stroke 0.6s ease 0s;`"
        >
          <animateTransform
            attributeName="transform"
            :dur="`4s`"
            type="rotate"
            :from="`${offset * 45} 50 50`"
            :to="`${offset * 45 + 360} 50 50`"
            repeatCount="indefinite"
          />
        </path>
      </svg>
    </div>
    <div class="content">
      <div class="value">{{ value }}%</div>
      <div class="name">{{ name }}</div>
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
    offset: {
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
.circle-wrapper-2 {
  display: flex;
  align-items: center;
  .loader {
    height: 50px;
    width: 50px;
  }
  .content {
    margin-left: 5px;
  }
  .name {
    color: #ccc;
    opacity: 0.6;
    margin-top: 5px;
    font-size: 14px;
  }
}
</style>
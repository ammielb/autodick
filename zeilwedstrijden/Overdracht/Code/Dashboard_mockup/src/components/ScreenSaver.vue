<template>
  <div class="container">
    <div class="row">
      <div class="flag-container">
        <GreenFlag :status="greenFlagStatus" />
        <div>
          <p v-if="greenFlagStatus">Klassevlag</p>
          <p v-else>Klassevlag</p>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="flag-container">
        <RedFlag :status="redFlagStatus" />
        <div>
          <p v-if="redFlagStatus">Zwarte vlag</p>
          <p v-else>Zwarte vlag</p>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="flag-container">
        <BlockedFlag :status="blockedFlagStatus" />
        <div>
          <p v-if="blockedFlagStatus">N vlag</p>
          <p v-else>N vlag</p>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import GreenFlag from './GreenFlag.vue';
import RedFlag from './RedFlag.vue';
import BlockedFlag from './BlockedFlag.vue';

export default {
  components: {
    GreenFlag,
    RedFlag,
    BlockedFlag,
  },
  data() {
    return {
      selectedNumbers: ['1', '1', '1'], // Default selected numbers
      greenFlagStatus: false,
      redFlagStatus: false,
      blockedFlagStatus: false
    };
  },
  methods: {
    selectNumber(index, number) {
      this.selectedNumbers[index - 1] = number.toString();
      this.checkFlags();
    },
    checkFlags() {
      this.greenFlagStatus = this.selectedNumbers[0] === '2';
      this.redFlagStatus = this.selectedNumbers[1] === '2';
      this.blockedFlagStatus = this.selectedNumbers[2] === '2';
    }
  }
};
</script>

<style scoped>
.container {
  border: 2px solid black;
  padding: 20px; /* Reduced padding */
}

.row {
  margin-bottom: 10px;
}

.flag-container {
  display: flex;
  align-items: center;
}

.flag {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  margin-right: 10px;
}

.green {
  background-color: green;
}

.red {
  background-color: red;
}

.blocked {
  background-color: gray;
}
</style>

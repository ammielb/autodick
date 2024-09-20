<template>
    <div id="horn">
        <p>Horn: </p>
        <p v-if="hornStatus">On</p>
        <p v-else>Off</p>
    </div>
</template>

<script>
export default {
    data() {
        return {
            hornStatus: false
        };
    },
    methods: {

        honk(status){
            this.hornStatus = status;
            console.log(status);
        },

        queuHonk(delay) {
            new Promise( resolve => setTimeout(() => {this.honk(1); resolve()}, delay));
            new Promise( resolve => setTimeout(() => {this.honk(0); resolve()}, delay+300));
        },

        requestHonk(times = 1){
            for (let i = 0; i < times; i++) {
                this.queuHonk(i * 600); 
            }
        }
    }
}
</script>


<style scoped>
    #horn{
        margin: 50px;
        width: 100px;
        height: 100px;
        border-radius: 50%;
        text-align: center;
        background-color: gray;
        color: white;
        display: flex;
        justify-content: center;
        align-items: center;
    }
</style>
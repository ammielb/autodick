<template>
    <div>
        <p>Log:</p>
        <ul id="messages">
        </ul>
    </div>

</template>

<script>
export default {
    methods: {
        log(msg){
            let message = document.createElement("li");
            const now = new Date();
            const console = document.getElementById("messages");
            message.textContent = `[${`${now.toLocaleDateString()} ${now.toLocaleTimeString()}`}] ${msg}`;
            document.getElementById("messages").appendChild(message);
            console.scrollTo(0, console.scrollHeight);
        }
    }
}
</script>

<style>
    #messages { overflow-y: scroll; max-height: 300px; list-style-type: none; margin: 0; padding: 0; background-color: rgba(0, 0, 0, 0.15);}
    #messages > li { padding: 0.5rem 1rem; display: flex;}
    #messages > li:nth-child(odd) { background: #3e3d32; color: white;}
</style>
// Allows a middleware to end a request.
// Good for proxying.

module.exports = (middleware, req, res) => {
  return new Promise((resolve, reject) => {
    let next = () => {
      resolve();
    };
    middleware(req, res, next);
  });
};
